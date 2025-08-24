module Background;

void Background::updateDraw(double deltaTime)
{
	if(!quad || animationTime > animationEndTime)
		return;

	animationTime += deltaTime;
	quad.setBackgroundColor(Color::pack(0, 0, 0, std::min(animationTime / animationEndTime * 240.0, 240.0)));
}

void Background::setVisible(bool visible)
{
	if(!visible)
	{
		if(!quad)
			return;

		quad = QuadPool::Reference();
		return;
	}

	if(quad)
		return;

	animationTime = 0.0;

	QuadData quadData{{Constants::screenWidth / 2.0f, Constants::screenHeight / 2.0f},{Color::pack(0, 0, 0, 0), Color::pack(0, 0, 0, 0)}, ' '};
	quadData.setScale(Constants::screenWidth, Constants::screenHeight);
	quad = quadPool.insert(quadData, QuadPool::ePopupBackground);
}
