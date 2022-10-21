#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#define M_PI        3.14159265358979323846f // matches value in gcc v2 math.h


constexpr int WINDOW_X = 256;
constexpr int WINDOW_Y = 240;

class Example : public olc::PixelGameEngine
{
public:

	enum class graph_t
	{
		STRAFE,
		VEL_SNAP
	};

	int32_t iGridGap;
	int32_t fGridSize;
	int32_t iGridOffsetFromFrame;
	int32_t fRadius;
	olc::Pixel bgColor;
	olc::Pixel gridColor;

	int32_t iScreenWidth;
	int32_t iScreenHeight;
	olc::vi2d vCenter;
	graph_t graph;


	float iSpeed;

	Example()
	{
		// Name your application
		sAppName = "Example";
	}

	void Graph_Strafes(bool justpressed);
	void Graph_FPS(bool justpressed);


public:
	bool OnUserCreate() override
	{
		fGridSize = 4;
		fRadius = 128;
		bgColor = olc::WHITE;
		gridColor = olc::BLACK;
		iSpeed = 190;
		graph = graph_t::STRAFE;

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		iScreenWidth = ScreenWidth();
		iScreenHeight = ScreenHeight();
		vCenter = { iScreenWidth / 2, iScreenHeight / 2 };
		Clear(bgColor);

		const auto DrawGrid = [this](olc::vi2d const pos, int size, float blocks, bool gridNumbers = false) -> void{

			float gap(0);

			for (float i = 0; i < size; i += blocks)
				gap++;

			iGridGap = gap;

			for (int x = pos.x; x < pos.x + size + blocks; x += gap)
				for (int y = pos.x; y < pos.y + size + blocks; y += gap) {

					DrawLine({ x, y }, { pos.x + size, y }, olc::BLACK);
					DrawLine({ x, y }, { x, pos.y + size }, olc::BLACK);

				}

			if (!gridNumbers)
				return;

			int y = pos.y;
			int tile = (blocks/2 );
			for (int x = pos.x - blocks; y < pos.y + blocks * gap; y += gap) {

				const std::string num = std::to_string(tile);

				DrawString(x - num.length() * 4*2, y, num, olc::BLACK);
				tile--;

			}

			int x = pos.x;
			tile = -(blocks / 2-1);
			for (int y = pos.y + size + blocks; x < pos.x + blocks * gap; x += gap) {

				const std::string num = std::to_string(tile);

				DrawString((x - num.length() * 4) + gap, y, num, olc::BLACK);
				tile++;

			}
				

		};
		bool justPressed = false;

		if (GetKey(olc::K1).bPressed) {
			graph = graph_t::STRAFE;
			justPressed = true;
		}
		else if (GetKey(olc::K2).bPressed) {
			graph = graph_t::VEL_SNAP;
			justPressed = true;
		}


		DrawGrid({ 32, 32 }, iScreenWidth - 32*2, fGridSize, true);
		DrawRect({ iScreenWidth / 2 - 1, iScreenHeight / 2 - 1 }, { 2, 2 }, olc::RED);

		Graph_Strafes(justPressed);
		Graph_FPS(justPressed);

		return true;
	}
};
void Example::Graph_Strafes(bool justPressed)
{
	if (graph != graph_t::STRAFE)
		return;

	

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

	if (justPressed) {
		fVelocity = 0;
		bStartSimulation = false;
	}

	if (GetKey(olc::HOME).bPressed) {
		fVelocity = iSpeed;
		bStartSimulation = !bStartSimulation;
	}


	//Clear(bgColor);

	if (bStartSimulation) {

		std::string const text = "Velocity: " + std::to_string(fVelocity);
		std::string const textB = "\nWidth: " + std::to_string(fAngle);

		DrawString({ iScreenWidth / 2 - (int)text.length() * 4, fGridSize / 2 }, text + textB, olc::BLACK);
	}
	else {
		std::string const text = "Strafe Simulation (HOME key)";
		DrawString({ iScreenWidth / 2 - (int)text.length() * 4, fGridSize / 2 }, text, olc::BLACK);
	}


	if (bStartSimulation) {
		const float fAngleOuter = fAngle - (45.f - fAngle);
		olc::vi2d vStrafe, vStrafeOuter, mid(vCenter.x, iScreenHeight - fGridSize*4);


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
}
void Example::Graph_FPS(bool justpressed)
{
	if (graph != graph_t::VEL_SNAP)
		return;

	static float fVelocity = 50;
	static bool bStartSimulation;

	if (justpressed) {
		fVelocity = 0;
		bStartSimulation = false;
	}

	if (GetKey(olc::HOME).bPressed) {
		fVelocity = 0;
		bStartSimulation = !bStartSimulation;
	}


	//Clear(bgColor);

	if (bStartSimulation) {

		std::string const text = "g_speed: " + std::to_string(fVelocity);

		DrawString({ iScreenWidth / 2 - (int)text.length() * 4, fGridSize / 2 }, text, olc::BLACK);
	}
	else {
		std::string const text = "Snap-Zone Simulation (HOME key)";
		DrawString({ iScreenWidth / 2 - (int)text.length() * 4, fGridSize / 2 }, text, olc::BLACK);
	}

	//std::string const __pos = std::string('(' + std::to_string(GetMouseX()) + ", " + std::to_string(GetMouseY()) + "), gap: " + std::to_string(iGridGap));
	//DrawString(0, 0, __pos, olc::BLACK);

	const auto ToGrid = [this](olc::vi2d& start) -> void{
		start.y += 8;
	};
	const auto GetPositionFromTile = [this](olc::vi2d tile) -> olc::vi2d
	{
		int leftSide = -fGridSize / 2;

		int distanceX = (int)abs(leftSide - tile.x);
		int distanceY = (int)abs(leftSide + tile.y);


		return { 32 + distanceX * iGridGap, 32 + distanceY * iGridGap };

	};
	const auto ClosestTileFromPosition = [this](olc::vi2d pos)->olc::vi2d
	{
		//pos.x -= (int)round(float(iGridGap) / 1.5f);
		//pos.y -= (int)round(float(iGridGap) / 1.5f);

		float closestX = (float)(pos.x + 32/2) / iGridGap;
		float closestY = (float)(pos.y + 32/2) / iGridGap;

		return { -fGridSize/2+1 + (-fGridSize / (fGridSize / 2) + (int)round(closestX)), 
				 fGridSize/2-1 - (-fGridSize / (fGridSize / 2) + (int)round(closestY))};

	};
	//olc::vi2d closestTile = ClosestTileFromPosition(GetMousePos());

	//std::string const _pos = std::string('(' + std::to_string(closestTile.x) + ", " + std::to_string(closestTile.y) + ')');
	//DrawString(0, 10, _pos, olc::BLACK);

	const float fps125 = ((fVelocity / 8) + 10) / iGridGap;
	const float fps200 = ((fVelocity / 5) + 10) / iGridGap;
	const float fps250 = ((fVelocity / 4) + 10) / iGridGap;
	const float fps333 = ((fVelocity / 3) + 10) / iGridGap;

	const float r_fps125 = round(fps125);
	const float r_fps200 = round(fps200);
	const float r_fps250 = round(fps250);
	const float r_fps333 = round(fps333);

	const float length125 = (90.f - ((fVelocity / 8) + 10)) - ((fVelocity / 8) + 10);
	const float length200 = ((fVelocity / 5) + 10) - (90.f - ((fVelocity / 5) + 10));
	const float length250 = ((fVelocity / 4) + 10) - (90.f - ((fVelocity / 4) + 10));
	const float length333 = ((fVelocity / 3) + 10) - (90.f - ((fVelocity / 3) + 10));

	const float radius = fVelocity >= 96 ? fmodf(fVelocity, 96) : fVelocity;
	//const float radius = 96.f;

	for (int j = 0; j < 4; j++) {
		float fps, r_fps, length;

		switch (j)
		{
		case 0:
			length = ((fVelocity / 8) + 10) - (90.f - ((fVelocity / 8) + 10));
			fps = ((fVelocity / 8) + 10) / iGridGap;
			//length -= 1;
			break;
		case 1:
			length = ((fVelocity / 5) + 10) - (90.f - ((fVelocity / 5) + 10));
			fps = ((fVelocity / 5) + 10) / iGridGap;
			break;
		case 2:
			length = ((fVelocity / 4) + 10) - (90.f - ((fVelocity / 4) + 10));
			fps = ((fVelocity / 4) + 10) / iGridGap;
			break;
		case 3: 
			fps = ((fVelocity / 3) + 10) / iGridGap;
			length = ((fVelocity / 3) + 10) - (90.f - ((fVelocity / 3) + 10));
			break;
		default: 
			break;
		}
		r_fps = round(fps);
		for (int i = 0; i < 4; i++) {


			olc::vi2d iOrgAngle, iRoundedAngle_c, iRoundedAngle;

			iOrgAngle.x = vCenter.x + sin((i * 90.f + (fps * iGridGap)) * M_PI / 180.f) * (radius);
			iOrgAngle.y = vCenter.y - cos((i * 90.f + (fps * iGridGap)) * M_PI / 180.f) * (radius);



			iRoundedAngle = GetPositionFromTile({ (int)r_fps , (int)r_fps });


			ToGrid(iOrgAngle);

			iRoundedAngle_c = ClosestTileFromPosition({ iOrgAngle });
			iRoundedAngle_c = GetPositionFromTile(iRoundedAngle_c);

			olc::Pixel px;
			px.a = 255;
			switch (j) {
			case 0:
				px.r = 255;
				px.g = 0;
				px.b = 255;
				break;
			case 1:
				px.r = 0;
				px.g = 0;
				px.b = 255;
				break;
			case 2:
				px.r = 0;
				px.g = 255;
				px.b = 0;
				break;
			case 3:
				px.r = 255;
				px.g = 0;
				px.b = 0;
				break;
			}

			DrawLine(iRoundedAngle_c, iOrgAngle, px);


			/// <summary>
			/// /////////////////////
			/// </summary>

			iOrgAngle.x = vCenter.x + sin((i * 90.f - length + (fps * iGridGap)) * M_PI / 180.f) * (radius);
			iOrgAngle.y = vCenter.y - cos((i * 90.f - length + (fps * iGridGap)) * M_PI / 180.f) * (radius);



			iRoundedAngle = GetPositionFromTile({ (int)r_fps , (int)r_fps });


			ToGrid(iOrgAngle);

			iRoundedAngle_c = ClosestTileFromPosition({ iOrgAngle });
			iRoundedAngle_c = GetPositionFromTile(iRoundedAngle_c);

			DrawLine(iRoundedAngle_c, iOrgAngle, px);


		}
	}
	olc::vi2d vCenter_c = vCenter;

	ToGrid(vCenter_c);
	DrawCircle(vCenter_c, radius, olc::RED);

	if (bStartSimulation)
		fVelocity += 0.002;

	//DrawString(0, 10, std::string("org333: " + std::to_string(fps333) + "\nrounded333: " + std::to_string(r_125_c.y)), olc::BLACK);

	//olc::vi2d closestTile = ClosestTileFromPosition(GetMousePos());

	//std::string const _pos = std::string('(' + std::to_string(closestTile.x) + ", " + std::to_string(closestTile.y) + ')');
	//DrawString(0, 0, _pos, olc::BLACK);

}
int main()
{
	Example demo;
	if (demo.Construct(WINDOW_X, WINDOW_Y, 4, 4))
		demo.Start();
	return 0;
}