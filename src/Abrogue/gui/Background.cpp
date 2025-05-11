module Background;

import GameSystems;

void Background::updateDraw(double deltaTime)
{
	if(!quad) 
		return;

	animationTime += deltaTime;

	//Fade the rectangle in over time
	quad.setBackgroundColor(Color::pack(0, 0, 0, std::min(animationTime / fadeTime * 240.0, 240.0)));
}

void Background::setVisible(bool visible)
{
	if(!visible)
	{
		quad = QuadPool::Reference();
		return;
	}

	animationTime = 0.0;

	//Create a black transparent rectangle that covers the whole screen
	QuadData quadData{{Constants::screenWidth / 2.0f, Constants::screenHeight / 2.0f}, {Color::pack(0, 0, 0, 0), Color::pack(0, 0, 0, 0)}, ' '};
	quadData.setScale(Constants::screenWidth, Constants::screenHeight);
	quad = quadPool.insert(quadData, QuadPool::ePopupBackground);
}