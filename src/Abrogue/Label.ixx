export module Label;

export import ObjectPools;

export class Label
{
public:
	Label(std::string_view text)
	{
		for(auto c : text)
		{
			quadReferences.emplace_back(QuadPool::insert(QuadData{{0.0f, 0.0f}, 1.0f,
														 {QuadData::packColor(255, 255, 255, 255), QuadData::packColor(255, 255, 255, 0)}, (uint32_t)c}));
		}
	}

private:
	std::vector<QuadPool::Reference> quadReferences;
};