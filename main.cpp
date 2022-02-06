// TODO:
//
// Add a template example type (eg: ArrayElement<T>)
// Check if the types (concrete or abstract) are in the right place
// See Design Patterns book and SO questions

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <numeric>
#include <stdexcept>

class AbstractVisitor;

class AbstractElement
{

public:

    virtual
    ~AbstractElement();

    virtual
    void Accept(AbstractVisitor &visitor) = 0;

};

AbstractElement::~AbstractElement()
{
    // I do nothing
}


//////////////////////////////////////////////////////////////////////
// Pre-emptive declaration of AbstractVisitor, used by classes below
//////////////////////////////////////////////////////////////////////

class SingleElement;
class ArrayElement;
class StringElement;

class AbstractVisitor
{

public:

    virtual
    ~AbstractVisitor()
    {

    }

    virtual void ProcessSingleElement(const SingleElement& element) = 0;
    virtual void ProcessArrayElement(const ArrayElement& element) = 0;
    virtual void ProcessStringElement(const StringElement& element) = 0;

};


class SingleElement : public AbstractElement
{

public:

    SingleElement(double value)
        : value(value)
    {
    }

    virtual
    ~SingleElement()
    {
        // I do nothing
    }

    double GetValue() const
    {
        return value;
    }

    void SetValue(const double value)
    {
        this->value = value;
    }

    void Accept(AbstractVisitor &visitor)
    {
        visitor.ProcessSingleElement(*this);
    }

private:

    double value;
};


class ArrayElement : public AbstractElement
{

public:

    ArrayElement(std::vector<double> value)
        : value(value)
    {
    }

    virtual
    ~ArrayElement()
    {
        // I do nothing
    }

    std::vector<double> GetValue() const
    {
        return value;
    }

    void SetValue(const std::vector<double> value)
    {
        this->value = value;
    }

    void Accept(AbstractVisitor &visitor)
    {
        visitor.ProcessArrayElement(*this);
    }

private:

    std::vector<double> value;

};


class StringElement : public AbstractElement
{

public:

    StringElement(std::string value)
        : value(value)
    {
    }

    virtual
    ~StringElement()
    {
        // I do nothing
    }

    std::string GetValue() const
    {
        return value;
    }

    void SetValue(const std::string value)
    {
        this->value = value;
    }

    void Accept(AbstractVisitor &visitor)
    {
        visitor.ProcessStringElement(*this);
    }

private:

    std::string value;

};


//////////////////////////////////////////////////////////
// Visitor classes which define the logic for operations
//////////////////////////////////////////////////////////


class SumVisitor : public AbstractVisitor
{

public:

    SumVisitor()
        : value{0.0}
    {
    }

    ~SumVisitor()
    {
        // I do nothing
    }

    void ProcessSingleElement(const SingleElement& element)
    {
        int v = element.GetValue();

        value += v;
    }

    void ProcessArrayElement(const ArrayElement& element)
    {
        std::vector<double> v = element.GetValue();

        double sum = std::accumulate(v.cbegin(), v.cend(), 0.0);

        value += sum;
    }

    void ProcessStringElement(const StringElement& element)
    {
        std::string v = element.GetValue();

        auto lambda = [](double sum, unsigned char ch)
        {
            if(std::isdigit(ch))
            {
                sum += static_cast<double>(ch - '0');
                return sum;
            }
            return sum;
        };

        double sum = std::accumulate(v.begin(), v.end(), 0.0, lambda);

        value += sum;
    }

    double GetValue() const
    {
        return value;
    }

    void Reset()
    {
        value = 0.0;
    }

private:

    double value;

};


class MultiplyVisitor : public AbstractVisitor
{

public:

    MultiplyVisitor()
        : value(1.0)
    {
    }

    ~MultiplyVisitor()
    {
        // I do nothing
    }

    void ProcessSingleElement(const SingleElement& element)
    {
        int v = element.GetValue();

        value *= v;
    }

    void ProcessArrayElement(const ArrayElement& element)
    {
        std::vector<double> v = element.GetValue();

        auto lambda = [](double product, double value)
        {
            return product * value;
        };

        double product = std::accumulate(v.cbegin(), v.cend(), 1.0, lambda);

        value *= product;
    }

    void ProcessStringElement(const StringElement& element)
    {
        std::string v = element.GetValue();

        auto lambda_multiply = [](double product, unsigned char ch)
        {
            if(std::isdigit(ch))
            {
                product *= static_cast<double>(ch - '0');
                return product;
            }
            return product;
        };

        double product = std::accumulate(v.begin(), v.end(), 1.0, lambda_multiply);

        value *= product;
    }

    double GetValue() const
    {
        return value;
    }

    void Reset()
    {
        value = 1.0;
    }

private:

    double value;
};


class XORVisitor : public AbstractVisitor
{

public:

    XORVisitor()
        : value{0}
    {
    }

    ~XORVisitor()
    {
        // I do nothing
    }

    void ProcessSingleElement(const SingleElement& element)
    {
        throw std::runtime_error("Error: Cannot apply XOR operation to SingleElement type");
    }

    void ProcessArrayElement(const ArrayElement& element)
    {
        throw std::runtime_error("Error: Cannot apply XOR operation to ArrayElement type");
    }

    void ProcessStringElement(const StringElement& element)
    {
        std::string v = element.GetValue();

        auto lambda_xor = [](unsigned char checksum, unsigned char ch)
        {
            return checksum ^ ch;
        };

        unsigned char checksum = std::accumulate(v.begin(), v.end(), 0, lambda_xor);

        value ^= checksum;
    }

    unsigned char GetValue() const
    {
        return value;
    }

    void Reset()
    {
        value = 0;
    }

private:

    unsigned char value;
};


int main(int argc, char *argv[])
{

    // Initialize some stuff

    std::vector<double> initial_values{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};

    std::list<SingleElement> single_element_list;

    std::transform(initial_values.cbegin(), initial_values.cend(),
        std::back_inserter(single_element_list), [](auto v){return SingleElement(v);});

    // This could be done in a similar way using transform,
    // as above
    std::list<ArrayElement> array_element_list
    {
        ArrayElement({1.0}),
        ArrayElement({2.0, 3.0}),
        ArrayElement({4.0, 5.0, 6.0}),
        ArrayElement({7.0, 8.0, 9.0})
    };
    
    StringElement string_element("Hello World 123456789\2");

    SumVisitor sum_visitor;
    MultiplyVisitor multiply_visitor;
    XORVisitor xor_visitor;

    ///////////////////////////////
    // Process SingleElement list
    ///////////////////////////////

    // Adapter class (functor)
    class ForEachAdapter
    {
    public:

        // The polymorhpism is retained in the member
        // variable reference-to-visitor
        ForEachAdapter(AbstractVisitor& visitor_type)
            : visitor_type(visitor_type)
        {
        }

        // The purpose of this class is simply to convert
        // the function call operator into a call to
        // element.Accept()
        void operator() (AbstractElement &element)
        {
            element.Accept(visitor_type);
        }

    private:

        AbstractVisitor &visitor_type;

    };

    // Each block of code which uses the Visitor classes is enclosed
    // in a try-catch block. An exception is only thrown when the XOR
    // Visitor is applied to the SingleElement or ArrayElement types.

    try
    {
        // This can also be done using a standard
        // range-based for-auto loop
        //
        //for(auto element : single_element_list)
        //{
        //    element.Accept(sum_visitor);
        //}

        // Here we use the adapter class with for_each, however
        // this can also be done using a lambda function.
        // See below for an example of how to do that
        ForEachAdapter adapter(sum_visitor);
        std::for_each(single_element_list.begin(), single_element_list.end(), adapter);
    }
    catch(const std::runtime_error &e)
    {
        std::cout << e.what() << std::endl;
    }

    try
    {
        // This version uses for_each in combination with a lambda
        // The lambda accomplishes the same thing as the functor,
        // however the functor is probably easier to understand
        // for those not familiar with lambdas.
        std::for_each(single_element_list.begin(), single_element_list.end(),
            [&multiply_visitor](AbstractElement &element)
            {
                element.Accept(multiply_visitor);
            }
        );
    }
    catch(const std::runtime_error &e)
    {
        std::cout << e.what() << std::endl;
    }

    try
    {
        std::for_each(single_element_list.begin(), single_element_list.end(),
            [&xor_visitor](AbstractElement &element)
            {
                element.Accept(xor_visitor);
            }
        );
    }
    catch(const std::runtime_error &e)
    {
        std::cout << e.what() << std::endl;
    }
    
    // Print result
    std::cout << "Sum of SingleElement list: " << sum_visitor.GetValue() << std::endl;
    std::cout << "Product of SingleElement list: " << multiply_visitor.GetValue() << std::endl;
    std::cout << "Checksum of SingleElement list: " << static_cast<int>(xor_visitor.GetValue())  << std::endl;
    sum_visitor.Reset();
    multiply_visitor.Reset();
    xor_visitor.Reset();

    //////////////////////////////
    // Process ArrayElement list
    //////////////////////////////

    try
    {
        std::for_each(array_element_list.begin(), array_element_list.end(), /*adapter*/
            [&sum_visitor](AbstractElement &element)
            {
                element.Accept(sum_visitor);
            }
        );
    }
    catch(const std::runtime_error &e)
    {
        std::cout << e.what() << std::endl;
    }

    try
    {
        std::for_each(array_element_list.begin(), array_element_list.end(), /*adapter*/
            [&multiply_visitor](AbstractElement &element)
            {
                element.Accept(multiply_visitor);
            }
        );
    }
    catch(const std::runtime_error &e)
    {
        std::cout << e.what() << std::endl;
    }

    try
    {
        std::for_each(array_element_list.begin(), array_element_list.end(), /*adapter*/
            [&xor_visitor](AbstractElement &element)
            {
                element.Accept(xor_visitor);
            }
        );
    }
    catch(const std::runtime_error &e)
    {
        std::cout << e.what() << std::endl;
    }

    // Print result
    std::cout << "Sum of ArrayElement list: " << sum_visitor.GetValue() << std::endl;
    std::cout << "Product of ArrayElement list: " << multiply_visitor.GetValue() << std::endl;
    std::cout << "Checksum of ArrayElement list: " << static_cast<int>(xor_visitor.GetValue())  << std::endl;
    sum_visitor.Reset();
    multiply_visitor.Reset();
    xor_visitor.Reset();

    //////////////////////////
    // Process StringElement
    //////////////////////////

    try
    {
        string_element.Accept(sum_visitor);
    }
    catch(const std::runtime_error &e)
    {
        std::cout << e.what() << std::endl;
    }

    try
    {
        string_element.Accept(multiply_visitor);
    }
    catch(const std::runtime_error &e)
    {
        std::cout << e.what() << std::endl;
    }

    try
    {
        string_element.Accept(xor_visitor);
    }
    catch(const std::runtime_error &e)
    {
        std::cout << e.what() << std::endl;
    }

    // Print result
    std::cout << "Sum of StringElement: " << sum_visitor.GetValue() << std::endl;
    std::cout << "Product of StringElement: " << multiply_visitor.GetValue() << std::endl;
    std::cout << "Checksum of StringElement: " << static_cast<int>(xor_visitor.GetValue()) << std::endl;
    sum_visitor.Reset();
    multiply_visitor.Reset();
    xor_visitor.Reset();


    return 0;
}