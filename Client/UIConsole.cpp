#include "UIConsole.h"

#include <streambuf>
#include <sstream>
#include <vector>
#include <iostream>
#include <deque>

#include "Components2D/Script/Script2D.h"

#include "imgui/imgui.h"

using namespace soft;

#include <pybind11/embed.h>

namespace py = pybind11;

#include "BindPython.h"

class PyStdErrOutStreamRedirect {
	py::object _stdout;
	py::object _stderr;
	py::object _stdout_buffer;
	py::object _stderr_buffer;
public:
	PyStdErrOutStreamRedirect() {
		auto sysm = py::module::import("sys");
		_stdout = sysm.attr("stdout");
		_stderr = sysm.attr("stderr");
		auto stringio = py::module::import("io").attr("StringIO");
		_stdout_buffer = stringio();  // Other filelike object can be used here as well, such as objects created by pybind11
		_stderr_buffer = stringio();
		sysm.attr("stdout") = _stdout_buffer;
		sysm.attr("stderr") = _stderr_buffer;
	}
	std::string stdoutString() {
		auto ret = py::str(_stdout_buffer.attr("getvalue")());
		_stdout_buffer.attr("truncate")(0);
		_stdout_buffer.attr("seek")(0);
		return ret;
	}
	std::string stderrString() {
		auto ret = py::str(_stderr_buffer.attr("getvalue")());
		_stderr_buffer.attr("truncate")(0);
		_stderr_buffer.attr("seek")(0);
		return ret;
	}
	~PyStdErrOutStreamRedirect() {
		auto sysm = py::module::import("sys");
		sysm.attr("stdout") = _stdout;
		sysm.attr("stderr") = _stderr;
	}
};

class UIConsoleImpl : public UIConsole
{
private:
	constexpr inline static size_t INPUT_BUF_SIZE = 8 * 1024;
	constexpr inline static size_t BUF_SIZE = 8 * 1024;

	friend class UIScript;

	//fot std::cout
	std::streambuf* m_coutbuf = 0;
	std::streambuf* m_cerrbuf = 0;
	std::stringstream m_outBuf;
	std::stringstream m_errBuf;

	////for printf
	//char m_tempBuffer1[BUF_SIZE] = {};
	//char m_tempBuffer2[BUF_SIZE] = {};
	//decltype(stdout) m_oldStdOut = 0;
	//decltype(stderr) m_oldStdErr = 0;
	//decltype(stdout) m_newStdOut = 0;
	//decltype(stderr) m_newStdErr = 0;

	//std::deque<std::string> m_outHistory;
	//std::deque<std::string> m_errHistory;
	// text, error?
	std::deque<std::pair<int, std::string>> m_logHistoryView;

	size_t m_logBufferSize = 1024;

	//std::vector<std::string> m_commandHistory;


	std::string m_input;
	std::string m_resizedInput;

	py::scoped_interpreter m_pyGuard{};
	py::object m_pyGlobal;
	PyStdErrOutStreamRedirect m_pyOut{};

	bool m_scrollToBottom = true;

public:
	inline UIConsoleImpl()
	{
		m_input.resize(INPUT_BUF_SIZE);
		m_resizedInput.reserve(INPUT_BUF_SIZE);

		//m_newStdOut = fmemopen(m_tempBuffer1, BUF_SIZE, "w");
		//m_newStdErr = fmemopen(m_tempBuffer1, BUF_SIZE, "w");

		RedirectStdOutput();

		m_pyGlobal = py::globals();
		BindPython();

		pybind11::gil_scoped_release release;
	};

	inline ~UIConsoleImpl()
	{
		RestoreStdOutput();
	}

	inline void RedirectStdOutput()
	{
		if (!m_isCaptureStdOut)
		{
			return;
		}

		m_coutbuf = std::cout.rdbuf();
		m_cerrbuf = std::cerr.rdbuf();

		std::cout.rdbuf(m_outBuf.rdbuf());
		std::cerr.rdbuf(m_errBuf.rdbuf());
	};

	inline void RestoreStdOutput()
	{
		if (!m_isCaptureStdOut)
		{
			return;
		}

		std::cout.rdbuf(m_coutbuf);
		std::cerr.rdbuf(m_cerrbuf);

		auto outstr = m_outBuf.str();
		if (!outstr.empty())
		{
			m_logHistoryView.push_back({ 0, outstr });
		}

		auto errstr = m_errBuf.str();
		if (!errstr.empty())
		{
			m_logHistoryView.push_back({ 1, errstr });
		}

		m_outBuf.str("");
		m_errBuf.str("");
	};

	inline void ClearOutput()
	{
		m_logHistoryView.clear();

		m_outBuf.str("");
		m_errBuf.str("");
	};

	/*inline void ClearCommand()
	{
		m_commandHistory.clear();
	};*/

	void RenderConsole(Scene2D* scene)
	{
		ImGui::SetNextWindowPos(ImVec2(5, 5));
		ImGui::Begin("Console");

		{
			pybind11::gil_scoped_acquire acquire;

			if (ImGui::Button("Run Script"))
			{
				m_resizedInput = &m_input[0];

				//py::scoped_interpreter guard{};
				try {
					py::exec(m_resizedInput.c_str());
				}
				catch (py::error_already_set& eas) {
					std::cerr << eas.what() << "\n";
					//eas.trace();
				}
			}

			auto outStr = m_pyOut.stdoutString();
			auto errStr = m_pyOut.stderrString();
			if (!outStr.empty())
			{
				m_outBuf << outStr;
			}

			if (!errStr.empty())
			{
				m_errBuf << errStr;
			}

			pybind11::gil_scoped_release release;
		}

		ImGui::SameLine();

		if (ImGui::Button("Clear"))
		{
			ClearOutput();
		}

		ImGui::SameLine();
		ImGui::Checkbox("View last log", &m_scrollToBottom);
		//ImGui::SameLine();
		//ImGui::Checkbox("Capture std::cout", &Global::Get().setting.isCaptureSTDCout);

		ImGui::Separator();

		RestoreStdOutput();
		RedirectStdOutput();

		if (m_logHistoryView.size() > m_logBufferSize)
		{
			while (m_logHistoryView.size() > m_logBufferSize)
			{
				m_logHistoryView.pop_front();
			}
		}

		const float height = ImGui::GetWindowHeight() / 2.0f;
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, height), false, ImGuiWindowFlags_HorizontalScrollbar);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
		for (auto& log : m_logHistoryView)
		{
			auto error = log.first;

			if (error)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
			}
			ImGui::TextUnformatted(log.second.c_str());

			if (error)
			{
				ImGui::PopStyleColor();
			}
		}
		ImGui::PopStyleVar();

		if (m_scrollToBottom)
		{
			ImGui::SetScrollHereY(1.0f);
		}

		ImGui::EndChild();
		ImGui::Separator();

		//(scriptEngine ? 120 : 80)
		ImGui::InputTextMultiline("##source", &m_input[0], INPUT_BUF_SIZE,
			ImVec2(-FLT_MIN, height - 50), ImGuiInputTextFlags_AllowTabInput);

		ImGui::End();
	}

};

UIConsole* UIConsole::New()
{
	return new UIConsoleImpl();
}

void UIConsole::Delete(UIConsole* console)
{
	delete ((UIConsoleImpl*)console);
}