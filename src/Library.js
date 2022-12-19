
'use strict'

const { OnigScanner } = require('../build/Release/onig_scanner.node')
const { OnigString } = require('../build/Release/onig_scanner.node')


const { max } = Math;


class OnigRegExp {

	constructor ( source ){

		this.scanner = new OnigScanner([ source ]);
		this.source = source;
	}


	captureIndicesForMatch ( string , match ){

		if( match == null )
			return null

		const { captureIndices } = match;

		string = this.scanner
			.convertToString(string);

		for ( const capture of captureIndices ){

			const { start , end } = capture;

			capture.match = string
				.slice(start,end);
		}

		return captureIndices
	}


	searchSync ( string , startPosition = 0 ){

		const match = this.scanner
			.findNextMatchSync(string,startPosition);

		return this.captureIndicesForMatch(string,match);
	}


	search ( string , startPosition = 0 , callback ){

		if(typeof startPosition === 'function'){
			callback = startPosition;
			startPosition = 0;
		}

		const onMatch = ( error , match ) => {

			if( typeof callback !== 'function' )
				return;

			const indices = this
				.captureIndicesForMatch(string,match);

			callback(error,indices);
		}

		return this.scanner
			.findNextMatch(string,startPosition,onMatch);
	}


	testSync ( string ){
		return this.searchSync(string) != null
	}


	test ( string , callback ){

		const onFind = ( error , result ) => {

			if( typeof callback !== 'function' )
				return

			callback(error,result != null);
		}

		return this.search(string,0,onFind);
	}
}



OnigScanner.prototype.findNextMatch =

	function ( string , startPosition = 0 , callback ){

		if(typeof startPosition === 'function'){
			callback = startPosition
			startPosition = 0
		}

		startPosition = this.convertToNumber(startPosition);
		string = this.convertToString(string);

		const onMatch = ( error , match ) => {

			if(match)
				match.scanner = this;

			return callback(error,match)
		}

		this._findNextMatch(string,startPosition,onMatch);
	}


OnigScanner.prototype.findNextMatchSync =

	function ( string , startPosition = 0 ){

		startPosition = this.convertToNumber(startPosition);
		string = this.convertToString(string);

		const match = this._findNextMatchSync(string,startPosition);

		if( match )
			match.scanner = this;

		return match
	}


OnigScanner.prototype.convertToString =

	function ( value ){

		if( value === undefined )
			return 'undefined'

		if( value === null )
			return 'null'

		if( value.constructor == OnigString )
			return value

		return value
			.toString()
	}


OnigScanner.prototype.convertToNumber =

	function ( value ){

		value = parseInt(value)

		if( ! isFinite(value) )
			value = 0;

		value = max(value,0)

		return value
	}


OnigString.prototype.substring =

	function ( start , end ) {
		return this.content
			.substring(start,end)
	}


OnigString.prototype.toString =

	function ( start , end ){
		return this.content
	}


Object.defineProperty(OnigString.prototype,'length',{

	get (){
		return this.content.length
	}
})

exports.OnigScanner = OnigScanner
exports.OnigRegExp = OnigRegExp
exports.OnigString = OnigString
