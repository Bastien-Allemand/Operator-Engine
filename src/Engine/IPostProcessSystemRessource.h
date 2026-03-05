#pragma once
class IPostProcessSystemRessource
{
	public:
	virtual ~IPostProcessSystemRessource() = default;
	virtual void Release() = 0;
};