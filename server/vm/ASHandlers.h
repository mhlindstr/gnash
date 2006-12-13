//   Copyright (C) 2005, 2006 Free Software Foundation, Inc.
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

// 
//

#ifndef __ASHANDLERS_H__
#define __ASHANDLERS_H__

#include <string>
#include <map>
#include <vector>
#include "action.h" // we should get rid of this probably
#include "as_environment.h" // for ensure_stack inline (must be inlined!)
#include "swf.h"


// Forward declarations
namespace gnash {
	class ActionExec;
}

namespace gnash {

namespace SWF { // gnash::SWF

typedef enum {
    ARG_NONE = 0,
    ARG_STR,
    // default hex dump, in case the format is unknown or unsupported
    ARG_HEX,
    ARG_U8,
    ARG_U16,
    ARG_S16,
    ARG_PUSH_DATA,
    ARG_DECL_DICT,
    ARG_FUNCTION2
} as_arg_t;

// @@strk@@ should we move this to .cpp file ? it's only
// use is within SWFHandlers, anyway...
typedef void (*action_callback_t)(ActionExec& thread);
//as_environment &env, action_buffer& code, size_t& PC);
class ActionHandler
{
public:
    ActionHandler();
    ActionHandler(action_type type, action_callback_t func);
    ActionHandler(action_type type, std::string name, 
                  action_callback_t func);
    ActionHandler(action_type type, std::string name, 
                  action_callback_t func, as_arg_t format);
    ActionHandler(action_type type, std::string name, 
                  action_callback_t func, as_arg_t format, int nargs);
    ~ActionHandler();

    /// Execute the action
    void execute(ActionExec& thread) const;

    void toggleDebug(bool state) const { _debug = state; }
    action_type getType()   const { return _type; }
    std::string getName()   const { return _name; }
    int getNumArgs()        const { return _stack_args; }
    as_arg_t getArgFormat() const { return _arg_format; }
private:
    action_type       _type;
    std::string       _name;
    action_callback_t _callback;
    mutable bool      _debug;
    int               _stack_args; // the number of args to pop from the stack
    as_arg_t          _arg_format;
};

/// A singleton containing the supported SWF Action handlers.
class SWFHandlers
{
public:

	/// TODO: use a vector as we can have at most 254 actions
	/// (127 w/out length, 127 with length, the high bit is
	///  used to distinguish the two types)
	//typedef std::map<action_type, ActionHandler> container_type;

	// Indexed by action id
	typedef std::vector<ActionHandler> container_type;

	/// Return the singleton instance of SWFHandlers class
	static const SWFHandlers& instance();

	/// Execute the action identified by 'type' action type
	void execute(action_type type, ActionExec& thread) const;

	void toggleDebug(bool state) { _debug = state; }

	size_t size() const { return get_handlers().size(); }

	action_type lastType() const
	{
		return ACTION_GOTOEXPRESSION;
		//return _handlers[ACTION_GOTOEXPRESSION].getType();
	}

	const ActionHandler &operator[] (action_type x) const
	{
		//return const_cast<ActionHandler>(_handlers[x]);
		return get_handlers()[x];
	}

	const char* action_name(action_type x) const;

private:

	static container_type & get_handlers();
	static std::vector<std::string> & get_property_names();

	// Ensure the stack has at least 'required' elements, fixing
	// it if required.
	// This is an inline to it can eventually be made a no-op
	// when gnash works and input SWFs are known to be valid.
	static void ensure_stack(as_environment& env, size_t required)
	{
		if ( env.stack_size() < required )
		{
			fix_stack_underrun(env, required);
		}
	}

	// Fill all the slots to reach the 'required' stack size
	// with undefined values. This method should *only* be
	// called by ensure_stack() above.
	static void fix_stack_underrun(as_environment& env, size_t required);

	/// Common code for ActionGetUrl and ActionGetUrl2
	//
	/// @see http://sswf.sourceforge.net/SWFalexref.html#action_get_url2
	/// @see http://sswf.sourceforge.net/SWFalexref.html#action_get_url
	///
	/// @param target
	///	the target window or _level1 to _level10
	///
	/// @param method
	///	0:NONE, 1:GET, 2:POST
	///
	static void CommonGetUrl(as_environment& env, 
			as_value target, const char* url,
			uint8_t method);

	/// Common code for SetTarget and SetTargetExpression
	//
	/// @see http://sswf.sourceforge.net/SWFalexref.html#action_set_target
	/// @see http://sswf.sourceforge.net/SWFalexref.html#action_get_dynamic
	///
	/// @param target_name
	///	The target name. If empty new target will be the main movie.
	///
	/// @param env
	///	The current execution environment.
	///
	static void CommonSetTarget(as_environment& env, 
			const std::string& target_name);

	static void ActionEnd(ActionExec& thread);
	static void ActionNextFrame(ActionExec& thread);
	static void ActionPrevFrame(ActionExec& thread);
	static void ActionPlay(ActionExec& thread);
	static void ActionStop(ActionExec& thread);
	static void ActionToggleQuality(ActionExec& thread);
	static void ActionStopSounds(ActionExec& thread);
	static void ActionGotoFrame(ActionExec& thread);
	static void ActionGetUrl(ActionExec& thread);
	static void ActionWaitForFrame(ActionExec& thread);
	static void ActionSetTarget(ActionExec& thread);
	static void ActionGotoLabel(ActionExec& thread);
	static void ActionAdd(ActionExec& thread);
	static void ActionSubtract(ActionExec& thread);
	static void ActionMultiply(ActionExec& thread);
	static void ActionDivide(ActionExec& thread);
	static void ActionEqual(ActionExec& thread);
	static void ActionLessThan(ActionExec& thread);
	static void ActionLogicalAnd(ActionExec& thread);
	static void ActionLogicalOr(ActionExec& thread);
	static void ActionLogicalNot(ActionExec& thread);
	static void ActionStringEq(ActionExec& thread);
	static void ActionStringLength(ActionExec& thread);
	static void ActionSubString(ActionExec& thread);
	static void ActionPop(ActionExec& thread);
	static void ActionInt(ActionExec& thread);
	static void ActionGetVariable(ActionExec& thread);
	static void ActionSetVariable(ActionExec& thread);
	static void ActionSetTargetExpression(ActionExec& thread);
	static void ActionStringConcat(ActionExec& thread);
	static void ActionGetProperty(ActionExec& thread);
	static void ActionSetProperty(ActionExec& thread);
	static void ActionDuplicateClip(ActionExec& thread);
	static void ActionRemoveClip(ActionExec& thread);
	static void ActionTrace(ActionExec& thread);
	static void ActionStartDragMovie(ActionExec& thread);
	static void ActionStopDragMovie(ActionExec& thread);
	static void ActionStringCompare(ActionExec& thread);
	static void ActionThrow(ActionExec& thread);
	static void ActionCastOp(ActionExec& thread);
	static void ActionImplementsOp(ActionExec& thread);
	static void ActionRandom(ActionExec& thread);
	static void ActionMbLength(ActionExec& thread);
	static void ActionOrd(ActionExec& thread);
	static void ActionChr(ActionExec& thread);
	static void ActionGetTimer(ActionExec& thread);
	static void ActionMbSubString(ActionExec& thread);
	static void ActionMbOrd(ActionExec& thread);
	static void ActionMbChr(ActionExec& thread);
	static void ActionWaitForFrameExpression(ActionExec& thread);
	static void ActionPushData(ActionExec& thread);
	static void ActionBranchAlways(ActionExec& thread);
	static void ActionGetUrl2(ActionExec& thread);
	static void ActionBranchIfTrue(ActionExec& thread);
	static void ActionCallFrame(ActionExec& thread);
	static void ActionGotoExpression(ActionExec& thread);
	static void ActionDelete(ActionExec& thread);
	static void ActionDelete2(ActionExec& thread);
	static void ActionVarEquals(ActionExec& thread);
	static void ActionCallFunction(ActionExec& thread);
	static void ActionReturn(ActionExec& thread);
	static void ActionModulo(ActionExec& thread);
	static void ActionNew(ActionExec& thread);
	static void ActionVar(ActionExec& thread);
	static void ActionInitArray(ActionExec& thread);
	static void ActionInitObject(ActionExec& thread);
	static void ActionTypeOf(ActionExec& thread);
	static void ActionTargetPath(ActionExec& thread);
	static void ActionEnumerate(ActionExec& thread);
	static void ActionNewAdd(ActionExec& thread);
	static void ActionNewLessThan(ActionExec& thread);
	static void ActionNewEquals(ActionExec& thread);
	static void ActionToNumber(ActionExec& thread);
	static void ActionToString(ActionExec& thread);
	static void ActionDup(ActionExec& thread);
	static void ActionSwap(ActionExec& thread);
	static void ActionGetMember(ActionExec& thread);
	static void ActionSetMember(ActionExec& thread);
	static void ActionIncrement(ActionExec& thread);
	static void ActionDecrement(ActionExec& thread);
	static void ActionCallMethod(ActionExec& thread);
	static void ActionNewMethod(ActionExec& thread);
	static void ActionInstanceOf(ActionExec& thread);
	static void ActionEnum2(ActionExec& thread);
	static void ActionBitwiseAnd(ActionExec& thread);
	static void ActionBitwiseOr(ActionExec& thread);
	static void ActionBitwiseXor(ActionExec& thread);
	static void ActionShiftLeft(ActionExec& thread);
	static void ActionShiftRight(ActionExec& thread);
	static void ActionShiftRight2(ActionExec& thread);
	static void ActionStrictEq(ActionExec& thread);
	static void ActionGreater(ActionExec& thread);
	static void ActionStringGreater(ActionExec& thread);
	static void ActionExtends(ActionExec& thread);
	static void ActionConstantPool(ActionExec& thread);
	static void ActionDefineFunction2(ActionExec& thread);
	static void ActionTry(ActionExec& thread);
	static void ActionWith(ActionExec& thread);
	static void ActionDefineFunction(ActionExec& thread);
	static void ActionSetRegister(ActionExec& thread);

	bool _debug;

	// Use the ::instance() method to get a reference
	SWFHandlers();

	// You won't destroy a singleton
	~SWFHandlers();

};


} // namespace gnash::SWF

} // namespace gnash

#endif // end of __ASHANDLERS_H__
