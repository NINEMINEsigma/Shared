
//Copyright And License : NINEMINE
//C++17 C11

#include"AlphaDiagram/ad_base.hpp"
#include"graph.h"

using namespace graph;

class test :ad::_Base
{
public:
	test() = default;
	test(int b) :a(b) {}
	int a = 0;
private:

};

class test_g :public ad::_BaseGenerator<test>
{
public:

	// Í¨¹ý _BaseGenerator ¼Ì³Ð
	virtual void init(test* _Right) override
	{
		//_Right->a = 10;
	}
private:



};


int main()
{
	//window_builder main_window;
	//main_window();
	try
	{
		test_g testg;
		test* xxx = new test(); //testg.generate(15);
		std::cout << xxx->a;
	}
	catch(ad::error_exception ex)
	{
		SetColorAndBackground(4, 0);
		std::cout <<"\nError Code:" << (int)ex;
		SetColorAndBackground(7, 0);
	}
}