
#include "onig-result.hpp"


OnigResult::OnigResult(
	OnigRegion * region ,
	int indexInScanner
) : region_(region)
  , indexInScanner(indexInScanner) {}


OnigResult::~OnigResult (){
	onig_region_free(region_,1);
}


auto OnigResult::Count () -> int {
	return region_ -> num_regs;
}


auto OnigResult::LocationAt ( int index ) -> int {

	const int bytes = * ( region_ -> beg + index );

	if( bytes < 0 )
		return 0;

	return bytes;
}


auto OnigResult::LengthAt ( int index ) -> int {

	const int bytes =
		* ( region_ -> end + index ) -
		* ( region_ -> beg + index ) ;

	if( bytes < 0 )
		return 0;

	return bytes;
}
