#ifndef Header_Oni_Result
#define Header_Oni_Result

#include "oniguruma.h"
#include "nan.h"


class OnigResult {

	private:

		OnigRegion * region_;
		int indexInScanner;

		// Disallow copying

		OnigResult & operator = ( const OnigResult & );
		OnigResult ( const OnigResult & );


	public:

		explicit OnigResult (
			OnigRegion * region ,
			int indexInScanner );

		~OnigResult ();

		auto LocationAt ( int ) -> int ;
		auto LengthAt ( int ) -> int ;
		auto Count () -> int ;

		auto Index () -> int
			{ return indexInScanner; }

		void SetIndex ( int newIndex )
			{ indexInScanner = newIndex; }

};


#endif
