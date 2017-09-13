/*
	This file is part of solidity.

	solidity is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	solidity is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with solidity.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * Unit tests for Solidity's ABI decoder.
 */

#include <functional>
#include <string>
#include <tuple>
#include <boost/test/unit_test.hpp>
#include <libsolidity/interface/Exceptions.h>
#include <test/libsolidity/SolidityExecutionFramework.h>

#include <test/libsolidity/ABITestsCommon.h>

using namespace std;
using namespace std::placeholders;
using namespace dev::test;

namespace dev
{
namespace solidity
{
namespace test
{

BOOST_FIXTURE_TEST_SUITE(ABIDecoderTest, SolidityExecutionFramework)

BOOST_AUTO_TEST_CASE(BOTH_ENCODERS_macro)
{
	// This tests that the "both decoders macro" at least runs twice and
	// modifies the source.
	string sourceCode;
	int runs = 0;
	BOTH_ENCODERS(runs++;)
	BOOST_CHECK(sourceCode == NewEncoderPragma);
	BOOST_CHECK_EQUAL(runs, 2);
}

BOOST_AUTO_TEST_CASE(value_types)
{
	string sourceCode = R"(
		contract C {
			function f(uint a, uint16 b, uint24 c, int24 d, bytes3 x, bool e, C g) returns (uint) {
				if (a != 1) return 1;
				if (b != 2) return 2;
				if (c != 3) return 3;
				if (d != 4) return 4;
				if (x != "abc") return 5;
				if (e != true) return 6;
				if (g != this) return 7;
				return 20;
			}
		}
	)";
	BOTH_ENCODERS(
		compileAndRun(sourceCode);
		BOOST_CHECK(callContractFunction(
			"f(uint256,uint16,uint24,int24,bytes3,bool,address)",
			1, 2, 3, 4, string("abc"), true, u160(m_contractAddress)
		) == encodeArgs(u256(20)));
	)
}

BOOST_AUTO_TEST_CASE(enums)
{
	string sourceCode = R"(
		contract C {
			enum E { A, B }
			function f(E e) returns (uint) {
				uint x;
				assembly { x := e }
			}
		}
	)";
	BOTH_ENCODERS(
		compileAndRun(sourceCode);
			cout << toHex(callContractFunction("f(uint8)", 0)) << endl;
		BOOST_CHECK(callContractFunction("f(uint8)", 0) == encodeArgs(u256(0)));
		BOOST_CHECK(callContractFunction("f(uint8)", 1) == encodeArgs(u256(1)));
		BOOST_CHECK(callContractFunction("f(uint8)", 2) == encodeArgs());
		BOOST_CHECK(callContractFunction("f(uint8)", u256(-1)) == encodeArgs());
	)
}

// test about incorrect input length
// tests about cleanup should use assembly
// More tests about overflow and stuff

BOOST_AUTO_TEST_SUITE_END()

}
}
} // end namespaces
