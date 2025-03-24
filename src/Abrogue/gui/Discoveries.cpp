module Discoveries;

import GameSystems;

void Discoveries::init()
{
	title.init("Discoveries", 56, 6, QuadPool::ePopup);

}

void Discoveries::setVisible(bool visible)
{
	title.setVisible(visible);
	if(visible)
	{
		QuadData backgroundQuad{{0.8889f, 0.5f}, {Helpers::packColor(0, 0, 0, 0), Helpers::packColor(0, 0, 0, 240)}, ' '};
		backgroundQuad.setScale(128.0f, 36.0f);
		background = quadPool.insert(backgroundQuad, QuadPool::ePopupBackground);
	}
	else
		background = QuadPool::Reference();
}