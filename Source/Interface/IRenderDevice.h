#pragma once

namespace Render
{
	class IRenderDevice
	{
	public:
		//Initialises the device
		virtual bool Init() = 0;

		virtual bool Render() = 0;

		virtual bool GetSceneManager() = 0;

	private:
	};
}