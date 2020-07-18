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
        break;
    case Type::R:
        mResistanceCount++;
	}
}

const Element& Elements::operator[](size_t index) const
{
	return mElements[index];
}

Element& Elements::operator[](size_t index)
{
    return const_cast<Element&>(
        static_cast<const Elements&>(*this)[index]
    );
}

size_t Elements::size() const
{
	return mElements.size();
}

size_t Elements::getCurrentSourceCount() const
{
    return mCurrentSourceCount;
}

size_t Elements::getVoltageSourceCount() const
{
    return mVoltageSourceCount;
}

size_t Elements::getResistanceCount() const
{
    return mResistanceCount;
}
