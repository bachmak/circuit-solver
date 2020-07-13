#include "../elements.h"

void Elements::add(const Element& element)
{
	mElements.push_back(element);

	switch (element.getType())
	{
	case Type::J:
		mCurrentSourceCount++;
		break;
	case Type::E:
		mVoltageSourceCount++;
	}
}

const Element& Elements::operator[](size_t index) const
{
	return mElements[index];
}

const size_t Elements::size() const
{
	return mElements.size();
}
