# OpenXR-Hpp: C++ Bindings for OpenXR

The goal of OpenXR C++ bindings is to improve the quality of life and type
safety for C++ developers interacting with the OpenXR API.

## Getting Started

Just include `<openxr/openxr.hpp>` and you're ready to use the C++ bindings.
For a more elegant usage, include files as you need them.

### Minimum Requirements

C++11 or newer is required. C++14 will increase the functionality available.

## Usage

### namespace xr

To avoid name collisions with other APIs, the C++ bindings reside in the `xr`
namespace by default.

The following additional naming conventions apply

* All functions, enums, handles, and structs have the `Xr`/`xr` prefix removed.
  In addition to this the first letter of functions is lower case.
  * `xrCreateInstance` can be accessed as `xr::createInstance`
  * `XrActionType` can be accessed as `xr::ActionType`
  * `XrViewState` can be accessed as `xr::ViewState`
* C enums are mapped to enum classes ("scoped enums") to provide compile time
  type safety. The names have been changed to CamelCase with the 'XR_' prefix
  and type infix removed.
  * In case the enum **type** is an extension, the extension suffix has been
    removed from the enum **values**.
  * In all other cases the extension suffix has not been removed.
  * Examples:
    * `XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO` is now
      `xr::ViewConfigurationType::PrimaryMono`. (No suffixes removed.)
    * `XR_VIEW_CONFIGURATION_TYPE_PRIMARY_QUAD_VARJO` is now
      `xr::ViewConfigurationType::PrimaryQuadVARJO` .(Extension suffix not
      removed because type is not from an extension.)
    * `XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT` is now
      `xr::DebugUtilsMessageSeverityFlagBitsEXT::Verbose`. (Type is an
      extension, so removed from value.)
* Flag bits are handled like enums with the addition that the `_BIT` suffix has
  also been removed.
  * `XR_SPACE_VELOCITY_LINEAR_VALID_BIT` is now
    `xr::SpaceVelocityFlagBits::LinearValid`

In some cases it might be necessary to move OpenXR-Hpp to a custom namespace.
This can be achieved by defining `OPENXR_HPP_NAMESPACE` before including
OpenXR-Hpp.

### Handles

OpenXR-Hpp declares a class for all handles to ensure full type safety and to
add support for member functions on handles. A member function has been added to
a handle class for each function which accepts the corresponding handle as first
parameter. Instead of `xrEnumerateReferenceSpaces(session, ...)` one can write
`session.enumerateReferenceSpaces(...)`.

### C/C++ Interop for Handles

@see handles

OpenXR-Hpp supports implicit conversions between C++ OpenXR handles and C OpenXR
handles. On 32-bit platforms all OpenXR handles are defined as `uint64_t`, thus
preventing type-conversion checks at compile time which would catch assignments
between incompatible handle types. Due to that, OpenXR-Hpp does not enable
implicit conversion for 32-bit platforms by default. In general, use of the free
function `get()` is recommended for use on all platforms if you need to get a
raw handle, etc. : `XrSession rawSession = get(session)`. For more about these
functions, see @ref utility_accessors.

If you're developing your code on a 64-bit platform, but want compile your code
for a 32-bit platform without adding the explicit getters you can define
`OPENXR_HPP_TYPESAFE_CONVERSION` to 1 in your build system or before including
`openxr.hpp`. On 64-bit platforms this define is set to 1 by default and can be
set to 0 to disable implicit conversions. However, this is not recommended.

### Flags

@see flags

The scoped enum-class feature adds type safety to the flags, but also prevents
using the flag bits as input for bitwise operations like `&` and `|` by default.

As solution OpenXR-Hpp provides a template class `xr::Flags` which brings the
standard operations like `&=`, `|=`, `&` and `|` to our scoped enums. Except for
the initialization with 0, this class behaves exactly like a normal bitmask with
the improvement that it is impossible to set bits not specified by the
corresponding enum by accident. Here are a few examples for the bitmask
handling:

```c++
xr::SwapchainUsageFlags iu1;      // initialize a bitmask with no bit set
xr::SwapchainUsageFlags iu2 = {}; // initialize a bitmask with no bit set
xr::SwapchainUsageFlags iu3 =
    xr::SwapchainUsageFlagBits::ColorAttachment; // initialize with a
                                                  // single value
xr::SwapchainUsageFlags iu4 =
    xr::SwapchainUsageFlagBits::ColorAttachment |
    xr::SwapchainUsageFlagBits::Sampled; // or two bits to get a bitmask
xr::SwapchainCreateInfo ci({} /* pass a flag without any bits set */,
                            ...);
```

### Enums

@see enums

Enums are projected similarly to flags, but without the bitwise operators
provided.

### Structs

@see structs

When constructing a handle in OpenXR, one usually has to create some
`CreateInfo` struct which describes the new handle. This can result in quite
lengthy code as can be seen in the following OpenXR C example:

```c++
XrSwapchainCreateInfo ci;
ci.type = XR_TYPE_SWAPCHAIN_CREATE_INFO;
ci.next = nullptr;
ci.usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
ci.format = VK_FORMAT_R8G8B8A8_SRGB;
ci.sampleCount = 1;
ci.width = width;
ci.height = height;
ci.faceCount = 1;
ci.arraySize = 1;
ci.mipCount = 1;
XrSwapchain swapchain;
xrCreateSwapchain(session, &ci, allocator, &swapchain));
```

There are two typical issues OpenXR developers encounter when filling out a
CreateInfo struct field by field:

* One or more fields are left uninitialized.
* `type` is incorrect.

Especially the first one is hard to detect.

OpenXR-Hpp provides constructors for all OpenXR structure types which accept one
parameter for each member variable. This way the compiler throws a compiler
error if a value has been forgotten. In addition to this, `type` is
automatically filled with the correct value and `next` set to a `nullptr` by
default. Here's how the same code looks with a constructor:

```c++
xr::SwapchainCreateInfo ci({}, xr::SwapchainUsageFlagBits::ColorAttachment,
                            VK_FORMAT, 1, width, height, 1, 1, 1);
xr::Swapchain swapchain = session.createSwapchain(ci);
```

With constructors for CreateInfo structures, one can also pass temporaries to OpenXR functions like this:

```c++
xr::Swapchain swapchain =
    session.createSwapchain({{},
                              xr::SwapchainUsageFlagBits::ColorAttachment,
                              VK_FORMAT,
                              1,
                              width,
                              height,
                              1,
                              1,
                              1});
```

### Return values, Error Codes & Exceptions

By default OpenXR-Hpp has exceptions enabled. This means that OpenXR-Hpp checks
the return code of each function call which returns a `XrResult`. If
`XrResult` is a failure a `std::runtime_error` will be thrown. In cases where
there are no success codes that aren't `xr::Result::Success`, there is no need
to return the error code any more, and the C++ bindings can now return the
actual desired return value, e.g. a OpenXR handle.

@see exceptions

If exceptions are disabled, or there are non-trivial success codes, the
`ResultValue` pair is returned. `ResultValue<SomeType>` is a struct holding a
`xr::Result` and a `SomeType`. This struct supports unpacking the return values
by using `std::tie` just like `std::tuple` or `std::pair`. If exception handling
is disabled by defining `OPENXR_HPP_NO_EXCEPTIONS`, it is provided for all
"enhanced mode" functions.

@see return_results

### Enumeration (Two-call idiom)

For the return value transformation, there's one special class of return values
which require special handling: enumeration calls using the "two-call idiom"
described in the "Buffer Size Parameters" part of the specification. In these
cases, when using the C API in C++ you usually have to write code like this:

```c++
std::vector<XrApiLayerProperties> properties;
uint32_t propertyCount = 0;
uint32_t propertyCapacity = 0;
XrResult result;
do
{
  // determine number of elements to query
  result = xrEnumerateApiLayerProperties( propertyCapacity, &propertyCount, nullptr ) );
  if ( ( result == XR_SUCCESS ) && propertyCount )
  {
    // allocate memory & query again
    properties.resize( propertyCount, {XR_TYPE_API_LAYER_PROPERTIES} );
    propertyCapacity = propertyCount;
    result = xrEumerateApiLayerProperties(
      propertyCapacity, &propertyCount, properties.data() );
  }
// it's possible that the count has changed, start again if properties was not big enough
} while ( result == XR_ERROR_SIZE_INSUFFICIENT );
// Trim off any extra unpopulated entries.
if (XR_SUCCEEDED(result)) properties.resize(propertyCount);
```

Since writing this loop over and over again is tedious and error prone, the C++
binding takes care of the enumeration so that you can just write:

```c++
std::vector<xr::ApiLayerProperties> properties = physicalDevice
    .enumerateApiLayerPropertiesToVector();
```

Note the addition of "ToVector" to the method name: this is to avoid some
ambiguous overloads.

### Custom assertions

All over the various headers, there are a couple of calls to an assert function.
By defining `OPENXR_HPP_ASSERT`, you can specify your own custom assert function
to be called instead.

@see config

### Extensions / Per-instance function pointers

@see dispatch

The OpenXR loader exposes only the OpenXR core functions directly as entry
points. Access to extensions requires dynamic loading of the corresponding
function pointers. OpenXR-Hpp provides a per-function dispatch mechanism by
accepting a dispatch class as last parameter in each function call. The dispatch
class must provide a callable type for each used OpenXR function. OpenXR-Hpp
provides two implementations. `DispatchLoaderStatic` is designed to expose the
core functions that are linked at build-time to the application. All core
functions defined in the headers _default_ to a `DispatchLoaderStatic` instance
when being called.

`DispatchLoaderDynamic`, on the other hand, exposes all the core functionality
_and_ all the known extensions. It can use lazy loading mechanism (use the
factory function `xr::DispatchLoaderDynamic::createFullyPopulated()` to eagerly
load) to populate function pointers that are called through it, and therefore
has a tiny amount of additional runtime overhead (a single null-pointer check
per function call) for functions called through it. This overhead is likely
outweighed by the performance benefit of being able to skip the loader's
trampoline function for the vast majority of function calls. Additionally, this
or something like it is the only way to safely share OpenXR state across
multiple modules of an application (e.g. a game engine and a game), and is the
only way to access extension-defined functions. Such extension functionality
declared in the C++ bindings will **not** have a default specified for the
dispatch parameter, so calling clients _must_ pass a dispatcher, unlike for core
functions.

For example:

```c++
xr::DispatchLoaderDynamic dispatch{ instance };
// ...
xr::DebugUtilsMessengerEXT messenger =
    instance.createDebugUtilsMessengerEXT({ severityFlags, typeFlags, debugCallback, userData },
                                          dispatch);
```

A client _should_ avoid instantiating a dynamic loader every time they make a
call which requires one. For example, the following is not recommended:

```c++
// Do **NOT** do this!
xr::DispatchLoaderDynamic dispatch{instance}; // used only for the one call below
xr::DebugUtilsMessengerEXT messenger =
    instance.createDebugUtilsMessengerEXT({ severityFlags, typeFlags, debugCallback, userData },
                                          dispatch);
```

```c++
// Do **NOT** do this!
xr::DebugUtilsMessengerEXT messenger =
    instance.createDebugUtilsMessengerEXT({ severityFlags, typeFlags, debugCallback, userData },
                                          xr::DispatchLoaderDynamic::createFullyPopulated(instance));
```

(Cannot lazy-load into a temporary.)

This code will work, but will load one or all function pointers (respectively)
into the dispatch object every time it's called. While not an issue for
infrequently called functions, if executed inside a loop or on a per-frame
basis, this can adversely impact performance.

Note that this can be configured.

@see config_dispatch

### Samples

## See Also

## Credits

Thanks [Markus Tavenrath](https://github.com/mtavenrath) and
[Andreas Süßenbach](https://github.com/asuessenbach) for their pioneering work
on the Vulkan C++ bindings, upon which much of the design of the OpenXR bindings
is based.

## License
