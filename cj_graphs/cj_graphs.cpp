#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#define M_PI        3.14159265358979323846f // matches value in gcc v2 math.h

class Example : public olc::PixelGameEngine
{
public:

	int32_t fGridSize;
	int32_t iGridOffset;
	int32_t fRadius;
	olc::Pixel bgColor;
	olc::Pixel gridColor;
	float iSpeed;

	Example()
	{
		// Name your application
		sAppName = "Example";
	}

public:
	bool OnUserCreate() override
	{
		fGridSize = 32;
		fRadius = 128;
		bgColor = olc::WHITE;
		gridColor = olc::BLACK;
		iSpeed = 190;
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{

		const auto GetStrafeLength = [this](const int32_t fps, const float fVelocity) -> float {

			float _iSpeed = iSpeed;
			const float fFPS = (float)fps;
			float accel = fFPS / iSpeed;

			if (fps == 125)
				accel = iSpeed / fFPS;

			if (_iSpeed < 190)
				_iSpeed = 190.f - (190.f - _iSpeed);

			return (acos((_iSpeed - accel) / fVelocity) * 180.f / M_PI) / 2;

		};

		static float fVelocity = iSpeed;
		const float fAngle = GetStrafeLength(1000, fVelocity);
		static bool bStartSimulation;

		if (GetKey(olc::HOME).bPressed) {
			fVelocity = iSpeed;
			bStartSimulation = !bStartSimulation;
		}

		const int32_t iScreenWidth = ScreenWidth();
		const int32_t iScreenHeight = ScreenHeight();
		const olc::vi2d vCenter(iScreenWidth / 2, iScreenHeight / 2);

		Clear(bgColor);

		if (bStartSimulation) {

			std::string const text = "Velocity: " + std::to_string(fVelocity);
			std::string const textB = "\nWidth: " + std::to_string(fAngle);

			DrawString({ iScreenWidth / 2 - (int)text.length() * 4, fGridSize / 2}, text + textB, olc::BLACK);
		}
		else {
			std::string const text = "Start Simulation (HOME key)";
			DrawString({ iScreenWidth / 2 - (int)text.length() * 4, fGridSize / 2}, "Start Simulation (HOME key)", olc::BLACK);
		}

		for (int xOffset = fGridSize; xOffset < iScreenWidth; xOffset += fGridSize) {

			for (int yOffset = fGridSize; yOffset < iScreenHeight; yOffset += fGridSize) {

				DrawLine({ 0 + fGridSize, yOffset }, { iScreenWidth - fGridSize, yOffset }, gridColor);
				DrawLine({ xOffset, 0 + fGridSize }, { xOffset, iScreenHeight - fGridSize / 2 }, gridColor);

				
				
			}
		}
		//DrawRect({ iScreenWidth / 2 - 1, iScreenHeight / 2 - 1 }, { 2, 2 }, olc::RED);
		

		if (bStartSimulation) {
			const float fAngleOuter = fAngle - (45.f - fAngle);
			olc::vi2d vStrafe, vStrafeOuter, mid(vCenter.x, iScreenHeight - fGridSize / 2);


			vStrafe.x = vCenter.x + sin(fAngle * M_PI / 180.f) * fRadius;
			vStrafe.y = iScreenHeight - fGridSize - cos(fAngle * M_PI / 180.f) * fRadius;

			vStrafeOuter.x = vCenter.x + sin(fAngleOuter * M_PI / 180.f) * fRadius;
			vStrafeOuter.y = iScreenHeight - fGridSize - cos(fAngleOuter * M_PI / 180.f) * fRadius;

			DrawLine(mid, vStrafe, olc::RED);
			DrawLine(mid, vStrafeOuter, olc::BLUE);
			//FillTriangle( mid, vStrafe, vStrafeOuter, olc::BLUE);


			vStrafe.x = vCenter.x - sin(fAngle * M_PI / 180.f) * fRadius;
			vStrafeOuter.x = vCenter.x - sin(fAngleOuter * M_PI / 180.f) * fRadius;
			vStrafeOuter.y = iScreenHeight - fGridSize - cos(fAngleOuter * M_PI / 180.f) * fRadius;

			DrawLine(mid, vStrafe, olc::RED);
			DrawLine(mid, vStrafeOuter, olc::BLUE);

			//FillTriangle( mid, vStrafe, vStrafeOuter, olc::BLUE);

			if (fVelocity < 1000.f)
				fVelocity += 0.025f;
			else if (fVelocity > 1000.f && fVelocity < 2500.f)
				fVelocity += 0.05f;
			else
				fVelocity += 0.1f;
		}
		return true;
	}
};

int main()
{
	Example demo;
	if (demo.Construct(256, 240, 3, 3))
		demo.Start();
	return 0;
}