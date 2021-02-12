#include "BERlength.h"
#include <algorithm>

BERlength::BERlength() : _size(0)
{
    for (auto& byte : _storage)
        byte = 0;
}

BERlength::BERlength(unsigned long value)
{
    for (auto& byte : _storage)
        byte = 0;
    if (value < 128)
    {
        _size = 1;
        _storage[0] = value & 0xFF;
        return;
    }
    unsigned char size = getNumberLength(value);
    _storage[0] = 0x80 | (size & 0x7F);
    for (int i = 1; i < size + 1; i++)
    {
        _storage[i] = (value >> 8 * (size - i)) & 0xFF;
    }
    _size = size + 1;
}

BERlength::BERlength(const char* data, size_t length)
{
    if (length == 0)
    {
        _size = 0;
        return;
    }
    if ((data[0] & 0x80) == 0)
    {
        _size = 1;
        _storage[0] = data[0];
    }
    else
    {
        _size = (data[0] & 0x7F) + 1;
        if (_size > length)
        {
            _size = 0;
            return;
        }
        _storage[0] = ((_size - 1) & 0x7F) | 0x80;
        for (int i = 1; i < _size; i++)
        {
            _storage[i] = data[i];
        }
    }
    normalize();
}

BERlength::BERlength(const BERlength& value)
{
    _size = value.getLength();
    for (int i = 0; i < _size; i++)
    {
        _storage[i] = value.getBytes()[i];
    }
}

size_t BERlength::getLength() const
{
    return _size;
}

const char* BERlength::getBytes() const
{
    return _storage;
}

bool BERlength::isZero() const
{
    if (_size == 0)
        return true;
    if (_storage[0] == 0)
        return true;
    bool result = true;
    for (int i = 1; i < _size; i++)
        if (_storage[i] != 0)
        {
            result = true;
            break;
        }
    return result;
}

bool BERlength::isShort() const
{
    if (_size == 1)
        return true;
    return false;
}

unsigned long BERlength::getValue() const
{
    if (_size - 1 > sizeof(unsigned long))
        return 0;
    else
    {
        if (_size == 1)
            return _storage[0] & 0x7F;

        unsigned long result = 0;
        for (int i = 1; i < _size; i++)
        {
            result += (_storage[i] << ((_size - i - 1) * 8)) & (0xFF << ((_size - i - 1) * 8));
        }
        return result;
    }
}

BERlength BERlength::operator+(unsigned long number)
{
    return *this + BERlength(number);
}

BERlength BERlength::operator+(BERlength number)
{
    unsigned char firstSingle = 0, secondSingle = 0;
    unsigned char* firstNumber, * secondNumber, * result;
    int firstSize, secondSize, resultSize;
    if (_size == 1)
    {
        firstSize = 1;
        firstNumber = &firstSingle;
        firstSingle = _storage[0] & 0x7F;
    }
    else
    {
        firstSize = _size - 1;
        firstNumber = (unsigned char*)&_storage[1];
    }
    if (number.getLength() == 1)
    {
        secondSize = 1;
        secondNumber = &secondSingle;
        secondSingle = number.getBytes()[0] & 0x7F;
    }
    else
    {
        secondSize = number.getLength() - 1;
        secondNumber = (unsigned char*)&number.getBytes()[1];
    }
    resultSize = std::max(firstSize, secondSize) + 2; // in case of carry and for leading octet
    result = new unsigned char[resultSize];
    int carry = 0;
    for (int i = 1; i < resultSize; i++)
    {
        int resultingByte = 0;
        if (firstSize - i >= 0)
            resultingByte += firstNumber[firstSize - i];
        if (secondSize - i >= 0)
            resultingByte += secondNumber[secondSize - i];
        resultingByte += carry;
        if (resultingByte > 255)
        {
            carry = 1;
            resultingByte -= 256;
        }
        else
            carry = 0;
        result[resultSize - i] = resultingByte & 0xFF;
    }
    result[0] = 0x80 | ((resultSize - 1) & 0x7F);
    BERlength finalResult((char*)result, resultSize);
    finalResult.normalize();
    delete[] result;
    return finalResult;
}

BERlength BERlength::operator-(unsigned long number)
{
    return *this - BERlength(number);
}

BERlength BERlength::operator-(BERlength number)
{
    unsigned char firstSingle = 0, secondSingle = 0;
    unsigned char* firstNumber, * secondNumber, * result;
    int firstSize, secondSize, resultSize;
    if (_size == 1)
    {
        firstSize = 1;
        firstNumber = &firstSingle;
        firstSingle = _storage[0] & 0x7F;
    }
    else
    {
        firstSize = _size - 1;
        firstNumber = (unsigned char*)&_storage[1];
    }
    if (number.getLength() == 1)
    {
        secondSize = 1;
        secondNumber = &secondSingle;
        secondSingle = number.getBytes()[0] & 0x7F;
    }
    else
    {
        secondSize = number.getLength() - 1;
        secondNumber = (unsigned char*)&number.getBytes()[1];
    }
    resultSize = std::max(firstSize, secondSize) + 1;
    result = new unsigned char[resultSize];
    int carry = 0;
    for (int i = 1; i < resultSize; i++)
    {
        int resultingByte = 0;
        if (firstSize - i >= 0)
            resultingByte += firstNumber[firstSize - i];
        if (secondSize - i >= 0)
            resultingByte -= secondNumber[secondSize - i];
        resultingByte -= carry;
        if (resultingByte < 0)
        {
            carry = 1;
            resultingByte += 256;
        }
        else
            carry = 0;
        result[resultSize - i] = resultingByte & 0xFF;
    }
    if (carry == 1)
    {
        resultSize = 1;
        result[0] = 0;
    }
    else
        result[0] = 0x80 | ((resultSize - 1) & 0x7F);
    BERlength finalResult((char*)result, resultSize);
    finalResult.normalize();
    delete[] result;
    return finalResult;
}

void BERlength::normalize()
{
    if (_size <= 1)
        return;
    if (_size == 2 && (unsigned char)_storage[1] < 128)
    {
        _size = 1;
        _storage[0] = (_storage[1] & 0x7F);
    }
    int numberToShift = 0;
    for (int i = 1; i < _size; i++)
    {
        if (_storage[i] == 0)
            numberToShift++;
        else
            break;
    }
    if (numberToShift == 0)
        return;
    if (numberToShift == _size - 1)
    {
        _size = 1;
        _storage[0] = 0;
    }
    for (int i = 1; i + numberToShift < _size; i++)
    {
        _storage[i] = _storage[i + numberToShift];
    }
    _size -= numberToShift;
    _storage[0] = ((_size - 1) & 0x7F) | 0x80;
}

size_t BERlength::getNumberLength(unsigned long number)
{
    size_t result = 0;
    if (number == 0)
        return result;
    for (int i = 1; i < sizeof(unsigned long); i++)
    {
        if (number < (1 << i * 8))
            return i;
    }

}

BERlength BERlength::shortToLong(unsigned char input)
{
    unsigned char number[2];
    number[0] = 128;
    number[1] = input;
    BERlength result((char*)number, 2);
    return result;
}
