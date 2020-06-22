/************************************************************************/
/* Zooid UI Configuration. Change UI Behavior here                      */
/************************************************************************/

// Put Additional Includes here if Needed
#include "FileSystem/DirectoryHelper.h"
#include "Utils/Array.h"
#include "Utils/HashMap.h"
#include "Utils/PrimitiveTypes.h"
#include "Input/Keys.h"

// Array Type that use in this code. Change to other type if needed
#define UIArray ZE::Array

// HasMap type that use in this code. Change to other type if needed
#define UIHashMap ZE::HashMap

// Stack Type that use in this code
#define UIStack ZE::Array

// HashMap Find
#define HashMapHas(hashmap, key) hashmap.hasKey(key)

// HashMap Find and assign
#define HashMapHasAndAssign(hashmap, key, defaultValue, out) \
	{ int hashMapIdx; out = hashmap.getIndex(key, hashMapIdx) ? hashmap.getValue(hashMapIdx) : defaultValue;  }

// Allocation function. Change to other if needed
#define UINEW(Class) new Class

// Deallocation function. Change to other if needed
#define UIFREE(object) delete object

// Rendering: Font rendering using font instancing
#define ZUI_USE_FONT_INSTANCING

// Rendering: Font instancing happen only per text not entire text.
#if defined(ZUI_USE_FONT_INSTANCING)
	#define ZUI_USE_SINGLE_TEXT_ONLY
#endif

// Rendering: Render rectangles and images using one rectangle and instance per object
#define ZUI_USE_RECT_INSTANCING

// Rendering: Rendering group by texture
//#define ZUI_GROUP_PER_TEXTURE

// Assets: Main Asset Folder - Where the UI engine looking for assets to use as default
#define RESOURCE_FOLDER PACKAGE_ROOT_PATH "/UI/"

// Assets: Default Font File (Relative to Resource Folder)
#define DEFAULT_FONT_PATH "Font/OpenSans-Regular.ttf"

// Keys: Special Keys Define
#define ZOOID_KEY_BACKSPACE	ZE::Key::Backspace
#define ZOOID_KEY_ENTER ZE::Key::Return
#define ZOOID_KEY_DELETE ZE::Key::Delete
#define ZOOID_KEY_ARROW_LEFT ZE::Key::LeftArrow
#define ZOOID_KEY_ARROW_RIGHT ZE::Key::RightArrow
#define ZOOID_KEY_ARROW_UP ZE::Key::UpArrow
#define ZOOID_KEY_ARROW_DOWN ZE::Key::DownArrow
#define ZOOID_KEY_HOME ZE::Key::Home
#define ZOOID_KEY_END ZE::Key::End

// Text Input: Length of temp text input (use as temp buffer)
#define TEXT_TEMP_LENGTH 246

// Data Types: Change Data Types if needed
namespace ZE
{
	typedef char UIChar;
	
	typedef UIChar* UIText;
}