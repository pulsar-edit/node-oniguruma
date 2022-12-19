#ifndef Header_Oni_Searcher
#define Header_Oni_Searcher

#include <vector>
#include "onig-string.hpp"
#include "onig-reg-exp.hpp"
#include "onig-result.hpp"

using ::std::shared_ptr;
using ::std::vector;


class OnigSearcher {

	private:

		vector<shared_ptr<OnigRegExp>> regExps;

	public:

		explicit OnigSearcher ( vector<shared_ptr<OnigRegExp>> regExps )
			: regExps(regExps) {}

		~OnigSearcher (){}

		auto Search ( OnigString * source , int charOffset )
			-> shared_ptr<OnigResult> ;

};

#endif
