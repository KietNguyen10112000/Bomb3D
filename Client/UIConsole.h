#pragma once

namespace soft 
{
	class Scene2D;
}

using namespace soft;

class UIConsole
{
public:
	bool m_isCaptureStdOut = false;

	static UIConsole* New();
	static void Delete(UIConsole* console);

	virtual ~UIConsole() {};
	virtual void RenderConsole(Scene2D* scene) = 0;

};