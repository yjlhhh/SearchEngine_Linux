//#include "GlobalDefine.hpp"
#include "PageLib.hpp"
//#include "WebPage.hpp"
#include "PageLibPreprocessor.hpp"

#include <stdio.h>
#include <time.h>

int main(void)
{
	wd::PageLib pagelib;

	time_t t1 = time(NULL);
	pagelib.create();
	pagelib.store();
	time_t t2 = time(NULL);
	printf("raw lib create and store time: %ld min\n", (t2 - t1));

	wd::PageLibPreprocessor libPreprocessor;
	libPreprocessor.doProcess();

	return 0;
}
