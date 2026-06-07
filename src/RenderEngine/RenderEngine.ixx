module;

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_hpp_macros.hpp>

#include "SDL3/SDL_vulkan.h"

export module RenderEngine;

import Helpers;
import ImageLoader;

import GLM;

import std;

using namespace std::literals;

/*
 * String conversions for Vulkan enums
 */
constexpr std::string_view enumToStr(VkResult result)
{
    switch (result)
    {
    case VK_SUCCESS:
        return "Success"sv;
    case VK_NOT_READY:
        return "NotReady"sv;
    case VK_TIMEOUT:
        return "Timeout"sv;
    case VK_EVENT_SET:
        return "EventSet"sv;
    case VK_EVENT_RESET:
        return "EventReset"sv;
    case VK_INCOMPLETE:
        return "Incomplete"sv;
    case VK_ERROR_OUT_OF_HOST_MEMORY:
        return "ErrorOutOfHostMemory"sv;
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        return "ErrorOutOfDeviceMemory"sv;
    case VK_ERROR_INITIALIZATION_FAILED:
        return "ErrorInitializationFailed"sv;
    case VK_ERROR_DEVICE_LOST:
        return "ErrorDeviceLost"sv;
    case VK_ERROR_MEMORY_MAP_FAILED:
        return "ErrorMemoryMapFailed"sv;
    case VK_ERROR_LAYER_NOT_PRESENT:
        return "ErrorLayerNotPresent"sv;
    case VK_ERROR_EXTENSION_NOT_PRESENT:
        return "ErrorExtensionNotPresent"sv;
    case VK_ERROR_FEATURE_NOT_PRESENT:
        return "ErrorFeatureNotPresent"sv;
    case VK_ERROR_INCOMPATIBLE_DRIVER:
        return "ErrorIncompatibleDriver"sv;
    case VK_ERROR_TOO_MANY_OBJECTS:
        return "ErrorTooManyObjects"sv;
    case VK_ERROR_FORMAT_NOT_SUPPORTED:
        return "ErrorFormatNotSupported"sv;
    case VK_ERROR_FRAGMENTED_POOL:
        return "ErrorFragmentedPool"sv;
    case VK_ERROR_UNKNOWN:
        return "ErrorUnknown"sv;
    case VK_ERROR_OUT_OF_POOL_MEMORY:
        return "ErrorOutOfPoolMemory"sv;
    case VK_ERROR_INVALID_EXTERNAL_HANDLE:
        return "ErrorInvalidExternalHandle"sv;
    case VK_ERROR_FRAGMENTATION:
        return "ErrorFragmentation"sv;
    case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
        return "ErrorInvalidOpaqueCaptureAddress"sv;
    case VK_PIPELINE_COMPILE_REQUIRED:
        return "PipelineCompileRequired"sv;
    case VK_ERROR_NOT_PERMITTED:
        return "ErrorNotPermitted"sv;
    case VK_ERROR_SURFACE_LOST_KHR:
        return "ErrorSurfaceLostKHR"sv;
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
        return "ErrorNativeWindowInUseKHR"sv;
    case VK_SUBOPTIMAL_KHR:
        return "SuboptimalKHR"sv;
    case VK_ERROR_OUT_OF_DATE_KHR:
        return "ErrorOutOfDateKHR"sv;
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
        return "ErrorIncompatibleDisplayKHR"sv;
    case VK_ERROR_VALIDATION_FAILED_EXT:
        return "ErrorValidationFailedEXT"sv;
    case VK_ERROR_INVALID_SHADER_NV:
        return "ErrorInvalidShaderNV"sv;
    case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:
        return "ErrorImageUsageNotSupportedKHR"sv;
    case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR:
        return "ErrorVideoPictureLayoutNotSupportedKHR"sv;
    case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR:
        return "ErrorVideoProfileOperationNotSupportedKHR"sv;
    case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR:
        return "ErrorVideoProfileFormatNotSupportedKHR"sv;
    case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR:
        return "ErrorVideoProfileCodecNotSupportedKHR"sv;
    case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR:
        return "ErrorVideoStdVersionNotSupportedKHR"sv;
    case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
        return "ErrorInvalidDrmFormatModifierPlaneLayoutEXT"sv;
    case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
        return "ErrorFullScreenExclusiveModeLostEXT"sv;
    case VK_THREAD_IDLE_KHR:
        return "ThreadIdleKHR"sv;
    case VK_THREAD_DONE_KHR:
        return "ThreadDoneKHR"sv;
    case VK_OPERATION_DEFERRED_KHR:
        return "OperationDeferredKHR"sv;
    case VK_OPERATION_NOT_DEFERRED_KHR:
        return "OperationNotDeferredKHR"sv;
    case VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR:
        return "ErrorInvalidVideoStdParametersKHR"sv;
    case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
        return "ErrorCompressionExhaustedEXT"sv;
    case VK_ERROR_INCOMPATIBLE_SHADER_BINARY_EXT:
        return "IncompatibleShaderBinaryEXT"sv;
    case VK_PIPELINE_BINARY_MISSING_KHR:
        return "PipelineBinaryMissingKHR"sv;
    case VK_ERROR_NOT_ENOUGH_SPACE_KHR:
        return "ErrorNotEnoughSpaceKHR"sv;
    default:
        return "unknown result"sv;
    }
}
constexpr std::string_view enumToStr(VkFormat format)
{
    switch (format)
    {
    case VK_FORMAT_UNDEFINED:
        return "Undefined"sv;
    case VK_FORMAT_R4G4_UNORM_PACK8:
        return "R4G4UnormPack8"sv;
    case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
        return "R4G4B4A4UnormPack16"sv;
    case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
        return "B4G4R4A4UnormPack16"sv;
    case VK_FORMAT_R5G6B5_UNORM_PACK16:
        return "R5G6B5UnormPack16"sv;
    case VK_FORMAT_B5G6R5_UNORM_PACK16:
        return "B5G6R5UnormPack16"sv;
    case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
        return "R5G5B5A1UnormPack16"sv;
    case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
        return "B5G5R5A1UnormPack16"sv;
    case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
        return "A1R5G5B5UnormPack16"sv;
    case VK_FORMAT_R8_UNORM:
        return "R8Unorm"sv;
    case VK_FORMAT_R8_SNORM:
        return "R8Snorm"sv;
    case VK_FORMAT_R8_USCALED:
        return "R8Uscaled"sv;
    case VK_FORMAT_R8_SSCALED:
        return "R8Sscaled"sv;
    case VK_FORMAT_R8_UINT:
        return "R8Uint"sv;
    case VK_FORMAT_R8_SINT:
        return "R8Sint"sv;
    case VK_FORMAT_R8_SRGB:
        return "R8Srgb"sv;
    case VK_FORMAT_R8G8_UNORM:
        return "R8G8Unorm"sv;
    case VK_FORMAT_R8G8_SNORM:
        return "R8G8Snorm"sv;
    case VK_FORMAT_R8G8_USCALED:
        return "R8G8Uscaled"sv;
    case VK_FORMAT_R8G8_SSCALED:
        return "R8G8Sscaled"sv;
    case VK_FORMAT_R8G8_UINT:
        return "R8G8Uint"sv;
    case VK_FORMAT_R8G8_SINT:
        return "R8G8Sint"sv;
    case VK_FORMAT_R8G8_SRGB:
        return "R8G8Srgb"sv;
    case VK_FORMAT_R8G8B8_UNORM:
        return "R8G8B8Unorm"sv;
    case VK_FORMAT_R8G8B8_SNORM:
        return "R8G8B8Snorm"sv;
    case VK_FORMAT_R8G8B8_USCALED:
        return "R8G8B8Uscaled"sv;
    case VK_FORMAT_R8G8B8_SSCALED:
        return "R8G8B8Sscaled"sv;
    case VK_FORMAT_R8G8B8_UINT:
        return "R8G8B8Uint"sv;
    case VK_FORMAT_R8G8B8_SINT:
        return "R8G8B8Sint"sv;
    case VK_FORMAT_R8G8B8_SRGB:
        return "R8G8B8Srgb"sv;
    case VK_FORMAT_B8G8R8_UNORM:
        return "B8G8R8Unorm"sv;
    case VK_FORMAT_B8G8R8_SNORM:
        return "B8G8R8Snorm"sv;
    case VK_FORMAT_B8G8R8_USCALED:
        return "B8G8R8Uscaled"sv;
    case VK_FORMAT_B8G8R8_SSCALED:
        return "B8G8R8Sscaled"sv;
    case VK_FORMAT_B8G8R8_UINT:
        return "B8G8R8Uint"sv;
    case VK_FORMAT_B8G8R8_SINT:
        return "B8G8R8Sint"sv;
    case VK_FORMAT_B8G8R8_SRGB:
        return "B8G8R8Srgb"sv;
    case VK_FORMAT_R8G8B8A8_UNORM:
        return "R8G8B8A8Unorm"sv;
    case VK_FORMAT_R8G8B8A8_SNORM:
        return "R8G8B8A8Snorm"sv;
    case VK_FORMAT_R8G8B8A8_USCALED:
        return "R8G8B8A8Uscaled"sv;
    case VK_FORMAT_R8G8B8A8_SSCALED:
        return "R8G8B8A8Sscaled"sv;
    case VK_FORMAT_R8G8B8A8_UINT:
        return "R8G8B8A8Uint"sv;
    case VK_FORMAT_R8G8B8A8_SINT:
        return "R8G8B8A8Sint"sv;
    case VK_FORMAT_R8G8B8A8_SRGB:
        return "R8G8B8A8Srgb"sv;
    case VK_FORMAT_B8G8R8A8_UNORM:
        return "B8G8R8A8Unorm"sv;
    case VK_FORMAT_B8G8R8A8_SNORM:
        return "B8G8R8A8Snorm"sv;
    case VK_FORMAT_B8G8R8A8_USCALED:
        return "B8G8R8A8Uscaled"sv;
    case VK_FORMAT_B8G8R8A8_SSCALED:
        return "B8G8R8A8Sscaled"sv;
    case VK_FORMAT_B8G8R8A8_UINT:
        return "B8G8R8A8Uint"sv;
    case VK_FORMAT_B8G8R8A8_SINT:
        return "B8G8R8A8Sint"sv;
    case VK_FORMAT_B8G8R8A8_SRGB:
        return "B8G8R8A8Srgb"sv;
    case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
        return "A8B8G8R8UnormPack32"sv;
    case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
        return "A8B8G8R8SnormPack32"sv;
    case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
        return "A8B8G8R8UscaledPack32"sv;
    case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
        return "A8B8G8R8SscaledPack32"sv;
    case VK_FORMAT_A8B8G8R8_UINT_PACK32:
        return "A8B8G8R8UintPack32"sv;
    case VK_FORMAT_A8B8G8R8_SINT_PACK32:
        return "A8B8G8R8SintPack32"sv;
    case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
        return "A8B8G8R8SrgbPack32"sv;
    case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
        return "A2R10G10B10UnormPack32"sv;
    case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
        return "A2R10G10B10SnormPack32"sv;
    case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
        return "A2R10G10B10UscaledPack32"sv;
    case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
        return "A2R10G10B10SscaledPack32"sv;
    case VK_FORMAT_A2R10G10B10_UINT_PACK32:
        return "A2R10G10B10UintPack32"sv;
    case VK_FORMAT_A2R10G10B10_SINT_PACK32:
        return "A2R10G10B10SintPack32"sv;
    case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
        return "A2B10G10R10UnormPack32"sv;
    case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
        return "A2B10G10R10SnormPack32"sv;
    case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
        return "A2B10G10R10UscaledPack32"sv;
    case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
        return "A2B10G10R10SscaledPack32"sv;
    case VK_FORMAT_A2B10G10R10_UINT_PACK32:
        return "A2B10G10R10UintPack32"sv;
    case VK_FORMAT_A2B10G10R10_SINT_PACK32:
        return "A2B10G10R10SintPack32"sv;
    case VK_FORMAT_R16_UNORM:
        return "R16Unorm"sv;
    case VK_FORMAT_R16_SNORM:
        return "R16Snorm"sv;
    case VK_FORMAT_R16_USCALED:
        return "R16Uscaled"sv;
    case VK_FORMAT_R16_SSCALED:
        return "R16Sscaled"sv;
    case VK_FORMAT_R16_UINT:
        return "R16Uint"sv;
    case VK_FORMAT_R16_SINT:
        return "R16Sint"sv;
    case VK_FORMAT_R16_SFLOAT:
        return "R16Sfloat"sv;
    case VK_FORMAT_R16G16_UNORM:
        return "R16G16Unorm"sv;
    case VK_FORMAT_R16G16_SNORM:
        return "R16G16Snorm"sv;
    case VK_FORMAT_R16G16_USCALED:
        return "R16G16Uscaled"sv;
    case VK_FORMAT_R16G16_SSCALED:
        return "R16G16Sscaled"sv;
    case VK_FORMAT_R16G16_UINT:
        return "R16G16Uint"sv;
    case VK_FORMAT_R16G16_SINT:
        return "R16G16Sint"sv;
    case VK_FORMAT_R16G16_SFLOAT:
        return "R16G16Sfloat"sv;
    case VK_FORMAT_R16G16B16_UNORM:
        return "R16G16B16Unorm"sv;
    case VK_FORMAT_R16G16B16_SNORM:
        return "R16G16B16Snorm"sv;
    case VK_FORMAT_R16G16B16_USCALED:
        return "R16G16B16Uscaled"sv;
    case VK_FORMAT_R16G16B16_SSCALED:
        return "R16G16B16Sscaled"sv;
    case VK_FORMAT_R16G16B16_UINT:
        return "R16G16B16Uint"sv;
    case VK_FORMAT_R16G16B16_SINT:
        return "R16G16B16Sint"sv;
    case VK_FORMAT_R16G16B16_SFLOAT:
        return "R16G16B16Sfloat"sv;
    case VK_FORMAT_R16G16B16A16_UNORM:
        return "R16G16B16A16Unorm"sv;
    case VK_FORMAT_R16G16B16A16_SNORM:
        return "R16G16B16A16Snorm"sv;
    case VK_FORMAT_R16G16B16A16_USCALED:
        return "R16G16B16A16Uscaled"sv;
    case VK_FORMAT_R16G16B16A16_SSCALED:
        return "R16G16B16A16Sscaled"sv;
    case VK_FORMAT_R16G16B16A16_UINT:
        return "R16G16B16A16Uint"sv;
    case VK_FORMAT_R16G16B16A16_SINT:
        return "R16G16B16A16Sint"sv;
    case VK_FORMAT_R16G16B16A16_SFLOAT:
        return "R16G16B16A16Sfloat"sv;
    case VK_FORMAT_R32_UINT:
        return "R32Uint"sv;
    case VK_FORMAT_R32_SINT:
        return "R32Sint"sv;
    case VK_FORMAT_R32_SFLOAT:
        return "R32Sfloat"sv;
    case VK_FORMAT_R32G32_UINT:
        return "R32G32Uint"sv;
    case VK_FORMAT_R32G32_SINT:
        return "R32G32Sint"sv;
    case VK_FORMAT_R32G32_SFLOAT:
        return "R32G32Sfloat"sv;
    case VK_FORMAT_R32G32B32_UINT:
        return "R32G32B32Uint"sv;
    case VK_FORMAT_R32G32B32_SINT:
        return "R32G32B32Sint"sv;
    case VK_FORMAT_R32G32B32_SFLOAT:
        return "R32G32B32Sfloat"sv;
    case VK_FORMAT_R32G32B32A32_UINT:
        return "R32G32B32A32Uint"sv;
    case VK_FORMAT_R32G32B32A32_SINT:
        return "R32G32B32A32Sint"sv;
    case VK_FORMAT_R32G32B32A32_SFLOAT:
        return "R32G32B32A32Sfloat"sv;
    case VK_FORMAT_R64_UINT:
        return "R64Uint"sv;
    case VK_FORMAT_R64_SINT:
        return "R64Sint"sv;
    case VK_FORMAT_R64_SFLOAT:
        return "R64Sfloat"sv;
    case VK_FORMAT_R64G64_UINT:
        return "R64G64Uint"sv;
    case VK_FORMAT_R64G64_SINT:
        return "R64G64Sint"sv;
    case VK_FORMAT_R64G64_SFLOAT:
        return "R64G64Sfloat"sv;
    case VK_FORMAT_R64G64B64_UINT:
        return "R64G64B64Uint"sv;
    case VK_FORMAT_R64G64B64_SINT:
        return "R64G64B64Sint"sv;
    case VK_FORMAT_R64G64B64_SFLOAT:
        return "R64G64B64Sfloat"sv;
    case VK_FORMAT_R64G64B64A64_UINT:
        return "R64G64B64A64Uint"sv;
    case VK_FORMAT_R64G64B64A64_SINT:
        return "R64G64B64A64Sint"sv;
    case VK_FORMAT_R64G64B64A64_SFLOAT:
        return "R64G64B64A64Sfloat"sv;
    case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
        return "B10G11R11UfloatPack32"sv;
    case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
        return "E5B9G9R9UfloatPack32"sv;
    case VK_FORMAT_D16_UNORM:
        return "D16Unorm"sv;
    case VK_FORMAT_X8_D24_UNORM_PACK32:
        return "X8D24UnormPack32"sv;
    case VK_FORMAT_D32_SFLOAT:
        return "D32Sfloat"sv;
    case VK_FORMAT_S8_UINT:
        return "S8Uint"sv;
    case VK_FORMAT_D16_UNORM_S8_UINT:
        return "D16UnormS8Uint"sv;
    case VK_FORMAT_D24_UNORM_S8_UINT:
        return "D24UnormS8Uint"sv;
    case VK_FORMAT_D32_SFLOAT_S8_UINT:
        return "D32SfloatS8Uint"sv;
    case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
        return "Bc1RgbUnormBlock"sv;
    case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
        return "Bc1RgbSrgbBlock"sv;
    case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
        return "Bc1RgbaUnormBlock"sv;
    case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
        return "Bc1RgbaSrgbBlock"sv;
    case VK_FORMAT_BC2_UNORM_BLOCK:
        return "Bc2UnormBlock"sv;
    case VK_FORMAT_BC2_SRGB_BLOCK:
        return "Bc2SrgbBlock"sv;
    case VK_FORMAT_BC3_UNORM_BLOCK:
        return "Bc3UnormBlock"sv;
    case VK_FORMAT_BC3_SRGB_BLOCK:
        return "Bc3SrgbBlock"sv;
    case VK_FORMAT_BC4_UNORM_BLOCK:
        return "Bc4UnormBlock"sv;
    case VK_FORMAT_BC4_SNORM_BLOCK:
        return "Bc4SnormBlock"sv;
    case VK_FORMAT_BC5_UNORM_BLOCK:
        return "Bc5UnormBlock"sv;
    case VK_FORMAT_BC5_SNORM_BLOCK:
        return "Bc5SnormBlock"sv;
    case VK_FORMAT_BC6H_UFLOAT_BLOCK:
        return "Bc6HUfloatBlock"sv;
    case VK_FORMAT_BC6H_SFLOAT_BLOCK:
        return "Bc6HSfloatBlock"sv;
    case VK_FORMAT_BC7_UNORM_BLOCK:
        return "Bc7UnormBlock"sv;
    case VK_FORMAT_BC7_SRGB_BLOCK:
        return "Bc7SrgbBlock"sv;
    case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
        return "Etc2R8G8B8UnormBlock"sv;
    case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
        return "Etc2R8G8B8SrgbBlock"sv;
    case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
        return "Etc2R8G8B8A1UnormBlock"sv;
    case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
        return "Etc2R8G8B8A1SrgbBlock"sv;
    case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
        return "Etc2R8G8B8A8UnormBlock"sv;
    case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
        return "Etc2R8G8B8A8SrgbBlock"sv;
    case VK_FORMAT_EAC_R11_UNORM_BLOCK:
        return "EacR11UnormBlock"sv;
    case VK_FORMAT_EAC_R11_SNORM_BLOCK:
        return "EacR11SnormBlock"sv;
    case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
        return "EacR11G11UnormBlock"sv;
    case VK_FORMAT_EAC_R11G11_SNORM_BLOCK:
        return "EacR11G11SnormBlock"sv;
    case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
        return "Astc4x4UnormBlock"sv;
    case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
        return "Astc4x4SrgbBlock"sv;
    case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
        return "Astc5x4UnormBlock"sv;
    case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
        return "Astc5x4SrgbBlock"sv;
    case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
        return "Astc5x5UnormBlock"sv;
    case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
        return "Astc5x5SrgbBlock"sv;
    case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
        return "Astc6x5UnormBlock"sv;
    case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
        return "Astc6x5SrgbBlock"sv;
    case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
        return "Astc6x6UnormBlock"sv;
    case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
        return "Astc6x6SrgbBlock"sv;
    case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
        return "Astc8x5UnormBlock"sv;
    case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
        return "Astc8x5SrgbBlock"sv;
    case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
        return "Astc8x6UnormBlock"sv;
    case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
        return "Astc8x6SrgbBlock"sv;
    case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
        return "Astc8x8UnormBlock"sv;
    case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
        return "Astc8x8SrgbBlock"sv;
    case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
        return "Astc10x5UnormBlock"sv;
    case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
        return "Astc10x5SrgbBlock"sv;
    case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
        return "Astc10x6UnormBlock"sv;
    case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
        return "Astc10x6SrgbBlock"sv;
    case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
        return "Astc10x8UnormBlock"sv;
    case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
        return "Astc10x8SrgbBlock"sv;
    case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
        return "Astc10x10UnormBlock"sv;
    case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
        return "Astc10x10SrgbBlock"sv;
    case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
        return "Astc12x10UnormBlock"sv;
    case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
        return "Astc12x10SrgbBlock"sv;
    case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
        return "Astc12x12UnormBlock"sv;
    case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:
        return "Astc12x12SrgbBlock"sv;
    case VK_FORMAT_G8B8G8R8_422_UNORM:
        return "G8B8G8R8422Unorm"sv;
    case VK_FORMAT_B8G8R8G8_422_UNORM:
        return "B8G8R8G8422Unorm"sv;
    case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
        return "G8B8R83Plane420Unorm"sv;
    case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:
        return "G8B8R82Plane420Unorm"sv;
    case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM:
        return "G8B8R83Plane422Unorm"sv;
    case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM:
        return "G8B8R82Plane422Unorm"sv;
    case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM:
        return "G8B8R83Plane444Unorm"sv;
    case VK_FORMAT_R10X6_UNORM_PACK16:
        return "R10X6UnormPack16"sv;
    case VK_FORMAT_R10X6G10X6_UNORM_2PACK16:
        return "R10X6G10X6Unorm2Pack16"sv;
    case VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16:
        return "R10X6G10X6B10X6A10X6Unorm4Pack16"sv;
    case VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16:
        return "G10X6B10X6G10X6R10X6422Unorm4Pack16"sv;
    case VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16:
        return "B10X6G10X6R10X6G10X6422Unorm4Pack16"sv;
    case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
        return "G10X6B10X6R10X63Plane420Unorm3Pack16"sv;
    case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
        return "G10X6B10X6R10X62Plane420Unorm3Pack16"sv;
    case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
        return "G10X6B10X6R10X63Plane422Unorm3Pack16"sv;
    case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
        return "G10X6B10X6R10X62Plane422Unorm3Pack16"sv;
    case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
        return "G10X6B10X6R10X63Plane444Unorm3Pack16"sv;
    case VK_FORMAT_R12X4_UNORM_PACK16:
        return "R12X4UnormPack16"sv;
    case VK_FORMAT_R12X4G12X4_UNORM_2PACK16:
        return "R12X4G12X4Unorm2Pack16"sv;
    case VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16:
        return "R12X4G12X4B12X4A12X4Unorm4Pack16"sv;
    case VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16:
        return "G12X4B12X4G12X4R12X4422Unorm4Pack16"sv;
    case VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16:
        return "B12X4G12X4R12X4G12X4422Unorm4Pack16"sv;
    case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
        return "G12X4B12X4R12X43Plane420Unorm3Pack16"sv;
    case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
        return "G12X4B12X4R12X42Plane420Unorm3Pack16"sv;
    case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
        return "G12X4B12X4R12X43Plane422Unorm3Pack16"sv;
    case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
        return "G12X4B12X4R12X42Plane422Unorm3Pack16"sv;
    case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
        return "G12X4B12X4R12X43Plane444Unorm3Pack16"sv;
    case VK_FORMAT_G16B16G16R16_422_UNORM:
        return "G16B16G16R16422Unorm"sv;
    case VK_FORMAT_B16G16R16G16_422_UNORM:
        return "B16G16R16G16422Unorm"sv;
    case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM:
        return "G16B16R163Plane420Unorm"sv;
    case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM:
        return "G16B16R162Plane420Unorm"sv;
    case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM:
        return "G16B16R163Plane422Unorm"sv;
    case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM:
        return "G16B16R162Plane422Unorm"sv;
    case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM:
        return "G16B16R163Plane444Unorm"sv;
    case VK_FORMAT_G8_B8R8_2PLANE_444_UNORM:
        return "G8B8R82Plane444Unorm"sv;
    case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16:
        return "G10X6B10X6R10X62Plane444Unorm3Pack16"sv;
    case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16:
        return "G12X4B12X4R12X42Plane444Unorm3Pack16"sv;
    case VK_FORMAT_G16_B16R16_2PLANE_444_UNORM:
        return "G16B16R162Plane444Unorm"sv;
    case VK_FORMAT_A4R4G4B4_UNORM_PACK16:
        return "A4R4G4B4UnormPack16"sv;
    case VK_FORMAT_A4B4G4R4_UNORM_PACK16:
        return "A4B4G4R4UnormPack16"sv;
    case VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK:
        return "Astc4x4SfloatBlock"sv;
    case VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK:
        return "Astc5x4SfloatBlock"sv;
    case VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK:
        return "Astc5x5SfloatBlock"sv;
    case VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK:
        return "Astc6x5SfloatBlock"sv;
    case VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK:
        return "Astc6x6SfloatBlock"sv;
    case VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK:
        return "Astc8x5SfloatBlock"sv;
    case VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK:
        return "Astc8x6SfloatBlock"sv;
    case VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK:
        return "Astc8x8SfloatBlock"sv;
    case VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK:
        return "Astc10x5SfloatBlock"sv;
    case VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK:
        return "Astc10x6SfloatBlock"sv;
    case VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK:
        return "Astc10x8SfloatBlock"sv;
    case VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK:
        return "Astc10x10SfloatBlock"sv;
    case VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK:
        return "Astc12x10SfloatBlock"sv;
    case VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK:
        return "Astc12x12SfloatBlock"sv;
    case VK_FORMAT_A1B5G5R5_UNORM_PACK16:
        return "A1B5G5R5UnormPack16"sv;
    case VK_FORMAT_A8_UNORM:
        return "A8Unorm"sv;
    case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG:
        return "Pvrtc12BppUnormBlockIMG"sv;
    case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG:
        return "Pvrtc14BppUnormBlockIMG"sv;
    case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG:
        return "Pvrtc22BppUnormBlockIMG"sv;
    case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG:
        return "Pvrtc24BppUnormBlockIMG"sv;
    case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG:
        return "Pvrtc12BppSrgbBlockIMG"sv;
    case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG:
        return "Pvrtc14BppSrgbBlockIMG"sv;
    case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG:
        return "Pvrtc22BppSrgbBlockIMG"sv;
    case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG:
        return "Pvrtc24BppSrgbBlockIMG"sv;
    case VK_FORMAT_R8_BOOL_ARM:
        return "R8BoolARM"sv;
    case VK_FORMAT_R16G16_SFIXED5_NV:
        return "R16G16Sfixed5NV"sv;
    case VK_FORMAT_R10X6_UINT_PACK16_ARM:
        return "R10X6UintPack16ARM"sv;
    case VK_FORMAT_R10X6G10X6_UINT_2PACK16_ARM:
        return "R10X6G10X6Uint2Pack16ARM"sv;
    case VK_FORMAT_R10X6G10X6B10X6A10X6_UINT_4PACK16_ARM:
        return "R10X6G10X6B10X6A10X6Uint4Pack16ARM"sv;
    case VK_FORMAT_R12X4_UINT_PACK16_ARM:
        return "R12X4UintPack16ARM"sv;
    case VK_FORMAT_R12X4G12X4_UINT_2PACK16_ARM:
        return "R12X4G12X4Uint2Pack16ARM"sv;
    case VK_FORMAT_R12X4G12X4B12X4A12X4_UINT_4PACK16_ARM:
        return "R12X4G12X4B12X4A12X4Uint4Pack16ARM"sv;
    case VK_FORMAT_R14X2_UINT_PACK16_ARM:
        return "R14X2UintPack16ARM"sv;
    case VK_FORMAT_R14X2G14X2_UINT_2PACK16_ARM:
        return "R14X2G14X2Uint2Pack16ARM"sv;
    case VK_FORMAT_R14X2G14X2B14X2A14X2_UINT_4PACK16_ARM:
        return "R14X2G14X2B14X2A14X2Uint4Pack16ARM"sv;
    case VK_FORMAT_R14X2_UNORM_PACK16_ARM:
        return "R14X2UnormPack16ARM"sv;
    case VK_FORMAT_R14X2G14X2_UNORM_2PACK16_ARM:
        return "R14X2G14X2Unorm2Pack16ARM"sv;
    case VK_FORMAT_R14X2G14X2B14X2A14X2_UNORM_4PACK16_ARM:
        return "R14X2G14X2B14X2A14X2Unorm4Pack16ARM"sv;
    case VK_FORMAT_G14X2_B14X2R14X2_2PLANE_420_UNORM_3PACK16_ARM:
        return "G14X2B14X2R14X22Plane420Unorm3Pack16ARM"sv;
    case VK_FORMAT_G14X2_B14X2R14X2_2PLANE_422_UNORM_3PACK16_ARM:
        return "G14X2B14X2R14X22Plane422Unorm3Pack16ARM"sv;
    default:
        return "unknown format"sv;
    }
}
constexpr std::string_view enumToStr(VkColorSpaceKHR colorSpace)
{
    switch (colorSpace)
    {
    case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR:
        return "SrgbNonlinear"sv;
    case VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT:
        return "DisplayP3NonlinearEXT"sv;
    case VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT:
        return "ExtendedSrgbLinearEXT"sv;
    case VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT:
        return "DisplayP3LinearEXT"sv;
    case VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT:
        return "DciP3NonlinearEXT"sv;
    case VK_COLOR_SPACE_BT709_LINEAR_EXT:
        return "Bt709LinearEXT"sv;
    case VK_COLOR_SPACE_BT709_NONLINEAR_EXT:
        return "Bt709NonlinearEXT"sv;
    case VK_COLOR_SPACE_BT2020_LINEAR_EXT:
        return "Bt2020LinearEXT"sv;
    case VK_COLOR_SPACE_HDR10_ST2084_EXT:
        return "Hdr10St2084EXT"sv;
    case VK_COLOR_SPACE_HDR10_HLG_EXT:
        return "Hdr10HlgEXT"sv;
    case VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT:
        return "AdobergbLinearEXT"sv;
    case VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT:
        return "AdobergbNonlinearEXT"sv;
    case VK_COLOR_SPACE_PASS_THROUGH_EXT:
        return "PassThroughEXT"sv;
    case VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT:
        return "ExtendedSrgbNonlinearEXT"sv;
    case VK_COLOR_SPACE_DISPLAY_NATIVE_AMD:
        return "DisplayNativeAMD"sv;
    default:
        return "unknown color space"sv;
    }
}
constexpr std::string_view enumToStr(VkPresentModeKHR presentMode)
{
    switch (presentMode)
    {
    case VK_PRESENT_MODE_IMMEDIATE_KHR:
        return "Immediate"sv;
    case VK_PRESENT_MODE_MAILBOX_KHR:
        return "Mailbox"sv;
    case VK_PRESENT_MODE_FIFO_KHR:
        return "Fifo"sv;
    case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
        return "FifoRelaxed"sv;
    case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
        return "SharedDemandRefresh"sv;
    case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
        return "SharedContinuousRefresh"sv;
    case VK_PRESENT_MODE_FIFO_LATEST_READY_KHR:
        return "FifoLatestReady"sv;
    default:
        return "unknown present mode"sv;
    }
}
constexpr std::string_view enumToStr(VkPhysicalDeviceType physicalDeviceType)
{
    switch (physicalDeviceType)
    {
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        return "Other"sv;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        return "IntegratedGpu"sv;
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        return "DiscreteGpu"sv;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        return "VirtualGpu"sv;
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        return "Cpu"sv;
    default:
        return "unknown physical device type"sv;
    }
}

[[nodiscard]] bool checkError(VkResult result, std::string_view successMessage, std::string_view errorMessage)
{
    if (result != VK_SUCCESS)
    {
        logger.logError("{}: {}", errorMessage, enumToStr(result));
        return true;
    }

    if (!successMessage.empty())
        logger.logInfo("{}", successMessage);

    return false;
}
template <class Value, class ResultValue>
[[nodiscard]] bool checkError(Value& value, ResultValue resultValue, std::string_view successMessage, std::string_view errorMessage)
{
    if (checkError(resultValue.result, successMessage, errorMessage))
        return true;

    value = std::move(resultValue.value);
    return false;
}

export class RenderConstants
{
public:
    static constexpr std::uint64_t screenTileHeight{36};
    static constexpr std::uint64_t screenTileWidth{36 * 2 * 16 / 9};
    static constexpr float tileScaleX{0.5f / screenTileHeight};
    static constexpr float tileScaleY{1.0f / screenTileHeight};
};

/*
 * Class for storing rendering data of a single quad
 */
export class QuadData
{
public:
    QuadData() = default;
    // Create Data with position in world coordinates and color
    QuadData(glm::vec2 position, glm::uvec2 colors, std::uint32_t glyph, glm::vec2 scale = {1.0, 1.0}) :
        position(position.x * RenderConstants::tileScaleX, position.y * RenderConstants::tileScaleY),
        scale(scale.x * RenderConstants::tileScaleX, scale.y * RenderConstants::tileScaleY), colors(colors), glyph(glyph)
    {}

    // Set position in world coordinates
    void setPosition(float positionX, float positionY) { position = {positionX * RenderConstants::tileScaleX, positionY * RenderConstants::tileScaleY}; }
    // Set scale in world coordinates
    void setScale(float scaleX, float scaleY) { scale = {RenderConstants::tileScaleX * scaleX, RenderConstants::tileScaleY * scaleY}; }
    // Set quad rotaion in radians
    void setRotation(float angle) { rotation = {std::cos(angle), std::sin(angle)}; }
    // Set quad rotation in precalculated cos and sin
    void setRotation(float cos, float sin) { rotation = {cos, sin}; }
    // Set packed glyph color
    void setColor(PackedColor packedColor) { colors[0] = packedColor; }
    // Set packed background color
    void setBackgroundColor(PackedColor packedColor) { colors[1] = packedColor; }
    // Set glyph index
    void setGlyph(std::uint32_t newGlyph) { glyph = newGlyph; }

private:
    glm::vec2 position{};           // Position in screenspace coordinates
    glm::vec2 scale{};              // Scale in screenspace coordinates
    glm::vec2 rotation{1.0f, 0.0f}; // Cos and Sin of rotation angle
    glm::uvec2 colors{};            // Packed glyph and background colors
    std::uint32_t glyph{};          // Index of drawn symbol
};

// Layers for defining draw order
export enum class QuadLayer { eMap, eItem, eEntity, eMapOverlay, ePopupBackground, ePopup, COUNT };

/*
 * Class for a bidirectional reference to quad data
 */
export template <QuadLayer>
class QuadReference
{
public:
    QuadReference() = default;

    // Swap reference indices and pointers in the pool
    QuadReference(QuadReference&& rhs) noexcept { *this = std::move(rhs); }
    QuadReference& operator=(QuadReference&& rhs) noexcept;

    // Destroy a reference to quad if it exists
    ~QuadReference();

    // Create a reference if it doesn't exist and set its data
    template <IsSameType<QuadData> T>
    void init(T&& quadData);
    // Destroy a reference if it exists
    void clear();

    operator bool() const { return index != -1; }

    // Modify an existing reference
    void setPosition(float positionX, float positionY) const;
    void setGlyph(std::uint8_t glyph) const;
    void setColor(std::uint32_t packedColor) const;
    void setBackgroundColor(std::uint32_t packedColor) const;

private:
    std::int64_t index{-1}; // Index of quad data in the pool
};

/*
 * Class for handling quad data storage
 */
export class QuadPool
{
    static constexpr Array<std::size_t, QuadLayer::COUNT> capacities{8192uz, 512uz, 512uz, 512uz, 512uz, 2048uz};

    template <QuadLayer layer>
    struct Storage
    {
        FixedVector<QuadData, capacities[layer]> data;
        FixedVector<QuadReference<layer>*, capacities[layer]> references;
    };

public:
    QuadPool() = default;

    template <QuadLayer layer>
    [[nodiscard]] auto getData()
    {
        return getStorage<layer>().data.getSpan();
    }
    [[nodiscard]] static constexpr auto getCapacity() { return std::accumulate(capacities.begin(), capacities.end(), 0uz); }

private:
    template <QuadLayer layer>
    [[nodiscard]] auto&& getStorage()
    {
        return std::get<(std::size_t)layer>(storage);
    }

    EnumTupleType<QuadLayer, Storage> storage;

    template <QuadLayer layer>
    friend class QuadReference;
};
export inline QuadPool quadPool;

/*
 * Class for handling vulkan library functions
 */
class VulkanLibrary
{
public:
    VulkanLibrary() = default;

    [[nodiscard]] bool init()
    {
        if (!libraryLoader.init("libvulkan.so"))
            return false;

        vkGetInstanceProcAddr = libraryLoader.getFunction<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");

        vkCreateInstance = (PFN_vkCreateInstance)vkGetInstanceProcAddr(nullptr, "vkCreateInstance");
        vkEnumerateInstanceExtensionProperties =
            (PFN_vkEnumerateInstanceExtensionProperties)vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceExtensionProperties");
        vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceLayerProperties");

        return true;
    }
    [[nodiscard]] bool init(VkInstance instance)
    {
        vkGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)vkGetInstanceProcAddr(instance, "vkGetDeviceProcAddr");

        vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        vkCreateDevice = (PFN_vkCreateDevice)vkGetInstanceProcAddr(instance, "vkCreateDevice");
        vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        vkDestroyInstance = (PFN_vkDestroyInstance)vkGetInstanceProcAddr(instance, "vkDestroyInstance");

        vkEnumerateDeviceExtensionProperties =
            (PFN_vkEnumerateDeviceExtensionProperties)vkGetInstanceProcAddr(instance, "vkEnumerateDeviceExtensionProperties");
        vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDevices");
        vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties");
        vkGetPhysicalDeviceQueueFamilyProperties =
            (PFN_vkGetPhysicalDeviceQueueFamilyProperties)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties");
        vkGetPhysicalDeviceSurfaceSupportKHR =
            (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
        vkGetPhysicalDeviceSurfaceFormatsKHR =
            (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
        vkGetPhysicalDeviceSurfacePresentModesKHR =
            (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR =
            (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
        vkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceMemoryProperties");
        vkGetPhysicalDeviceFeatures2 = (PFN_vkGetPhysicalDeviceFeatures2)vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFeatures2");

        return true;
    }
    [[nodiscard]] bool init(VkDevice device)
    {
        vkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)vkGetDeviceProcAddr(device, "vkCreateSwapchainKHR");
        vkCreateImageView = (PFN_vkCreateImageView)vkGetDeviceProcAddr(device, "vkCreateImageView");
        vkCreateRenderPass = (PFN_vkCreateRenderPass)vkGetDeviceProcAddr(device, "vkCreateRenderPass");
        vkCreateFramebuffer = (PFN_vkCreateFramebuffer)vkGetDeviceProcAddr(device, "vkCreateFramebuffer");
        vkCreateShaderModule = (PFN_vkCreateShaderModule)vkGetDeviceProcAddr(device, "vkCreateShaderModule");
        vkCreateBuffer = (PFN_vkCreateBuffer)vkGetDeviceProcAddr(device, "vkCreateBuffer");
        vkCreateSemaphore = (PFN_vkCreateSemaphore)vkGetDeviceProcAddr(device, "vkCreateSemaphore");
        vkCreateFence = (PFN_vkCreateFence)vkGetDeviceProcAddr(device, "vkCreateFence");
        vkCreateCommandPool = (PFN_vkCreateCommandPool)vkGetDeviceProcAddr(device, "vkCreateCommandPool");
        vkCreateImage = (PFN_vkCreateImage)vkGetDeviceProcAddr(device, "vkCreateImage");
        vkCreateSampler = (PFN_vkCreateSampler)vkGetDeviceProcAddr(device, "vkCreateSampler");
        vkCreateDescriptorSetLayout = (PFN_vkCreateDescriptorSetLayout)vkGetDeviceProcAddr(device, "vkCreateDescriptorSetLayout");
        vkCreateDescriptorPool = (PFN_vkCreateDescriptorPool)vkGetDeviceProcAddr(device, "vkCreateDescriptorPool");
        vkCreatePipelineLayout = (PFN_vkCreatePipelineLayout)vkGetDeviceProcAddr(device, "vkCreatePipelineLayout");
        vkCreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines)vkGetDeviceProcAddr(device, "vkCreateGraphicsPipelines");

        vkAllocateMemory = (PFN_vkAllocateMemory)vkGetDeviceProcAddr(device, "vkAllocateMemory");
        vkMapMemory = (PFN_vkMapMemory)vkGetDeviceProcAddr(device, "vkMapMemory");
        vkUnmapMemory = (PFN_vkUnmapMemory)vkGetDeviceProcAddr(device, "vkUnmapMemory");
        vkGetDeviceQueue = (PFN_vkGetDeviceQueue)vkGetDeviceProcAddr(device, "vkGetDeviceQueue");
        vkGetImageMemoryRequirements = (PFN_vkGetImageMemoryRequirements)vkGetDeviceProcAddr(device, "vkGetImageMemoryRequirements");
        vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)vkGetDeviceProcAddr(device, "vkGetSwapchainImagesKHR");
        vkBindBufferMemory = (PFN_vkBindBufferMemory)vkGetDeviceProcAddr(device, "vkBindBufferMemory");
        vkBindImageMemory = (PFN_vkBindImageMemory)vkGetDeviceProcAddr(device, "vkBindImageMemory");
        vkAllocateCommandBuffers = (PFN_vkAllocateCommandBuffers)vkGetDeviceProcAddr(device, "vkAllocateCommandBuffers");
        vkBeginCommandBuffer = (PFN_vkBeginCommandBuffer)vkGetDeviceProcAddr(device, "vkBeginCommandBuffer");
        vkQueueSubmit = (PFN_vkQueueSubmit)vkGetDeviceProcAddr(device, "vkQueueSubmit");
        vkGetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements)vkGetDeviceProcAddr(device, "vkGetBufferMemoryRequirements");
        vkFreeCommandBuffers = (PFN_vkFreeCommandBuffers)vkGetDeviceProcAddr(device, "vkFreeCommandBuffers");
        vkCmdPipelineBarrier = (PFN_vkCmdPipelineBarrier)vkGetDeviceProcAddr(device, "vkCmdPipelineBarrier");
        vkEndCommandBuffer = (PFN_vkEndCommandBuffer)vkGetDeviceProcAddr(device, "vkEndCommandBuffer");
        vkQueueWaitIdle = (PFN_vkQueueWaitIdle)vkGetDeviceProcAddr(device, "vkQueueWaitIdle");
        vkCmdCopyBufferToImage = (PFN_vkCmdCopyBufferToImage)vkGetDeviceProcAddr(device, "vkCmdCopyBufferToImage");
        vkAllocateDescriptorSets = (PFN_vkAllocateDescriptorSets)vkGetDeviceProcAddr(device, "vkAllocateDescriptorSets");
        vkUpdateDescriptorSets = (PFN_vkUpdateDescriptorSets)vkGetDeviceProcAddr(device, "vkUpdateDescriptorSets");
        vkGetBufferDeviceAddress = (PFN_vkGetBufferDeviceAddress)vkGetDeviceProcAddr(device, "vkGetBufferDeviceAddress");
        vkWaitForFences = (PFN_vkWaitForFences)vkGetDeviceProcAddr(device, "vkWaitForFences");
        vkResetFences = (PFN_vkResetFences)vkGetDeviceProcAddr(device, "vkResetFences");
        vkAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)vkGetDeviceProcAddr(device, "vkAcquireNextImageKHR");
        vkResetCommandBuffer = (PFN_vkResetCommandBuffer)vkGetDeviceProcAddr(device, "vkResetCommandBuffer");
        vkCmdBeginRenderPass = (PFN_vkCmdBeginRenderPass)vkGetDeviceProcAddr(device, "vkCmdBeginRenderPass");
        vkCmdBindPipeline = (PFN_vkCmdBindPipeline)vkGetDeviceProcAddr(device, "vkCmdBindPipeline");
        vkCmdSetViewport = (PFN_vkCmdSetViewport)vkGetDeviceProcAddr(device, "vkCmdSetViewport");
        vkCmdSetScissor = (PFN_vkCmdSetScissor)vkGetDeviceProcAddr(device, "vkCmdSetScissor");
        vkCmdPushConstants = (PFN_vkCmdPushConstants)vkGetDeviceProcAddr(device, "vkCmdPushConstants");
        vkCmdBindDescriptorSets = (PFN_vkCmdBindDescriptorSets)vkGetDeviceProcAddr(device, "vkCmdBindDescriptorSets");
        vkCmdDraw = (PFN_vkCmdDraw)vkGetDeviceProcAddr(device, "vkCmdDraw");
        vkCmdEndRenderPass = (PFN_vkCmdEndRenderPass)vkGetDeviceProcAddr(device, "vkCmdEndRenderPass");
        vkDeviceWaitIdle = (PFN_vkDeviceWaitIdle)vkGetDeviceProcAddr(device, "vkDeviceWaitIdle");
        vkFreeMemory = (PFN_vkFreeMemory)vkGetDeviceProcAddr(device, "vkFreeMemory");
        vkQueuePresentKHR = (PFN_vkQueuePresentKHR)vkGetDeviceProcAddr(device, "vkQueuePresentKHR");

        vkDestroyPipeline = (PFN_vkDestroyPipeline)vkGetDeviceProcAddr(device, "vkDestroyPipeline");
        vkDestroyBuffer = (PFN_vkDestroyBuffer)vkGetDeviceProcAddr(device, "vkDestroyBuffer");
        vkDestroyFence = (PFN_vkDestroyFence)vkGetDeviceProcAddr(device, "vkDestroyFence");
        vkDestroySemaphore = (PFN_vkDestroySemaphore)vkGetDeviceProcAddr(device, "vkDestroySemaphore");
        vkDestroyDevice = (PFN_vkDestroyDevice)vkGetDeviceProcAddr(device, "vkDestroyDevice");
        vkDestroyPipelineLayout = (PFN_vkDestroyPipelineLayout)vkGetDeviceProcAddr(device, "vkDestroyPipelineLayout");
        vkDestroyDescriptorPool = (PFN_vkDestroyDescriptorPool)vkGetDeviceProcAddr(device, "vkDestroyDescriptorPool");
        vkDestroyDescriptorSetLayout = (PFN_vkDestroyDescriptorSetLayout)vkGetDeviceProcAddr(device, "vkDestroyDescriptorSetLayout");
        vkDestroySampler = (PFN_vkDestroySampler)vkGetDeviceProcAddr(device, "vkDestroySampler");
        vkDestroyImageView = (PFN_vkDestroyImageView)vkGetDeviceProcAddr(device, "vkDestroyImageView");
        vkDestroyImage = (PFN_vkDestroyImage)vkGetDeviceProcAddr(device, "vkDestroyImage");
        vkDestroyCommandPool = (PFN_vkDestroyCommandPool)vkGetDeviceProcAddr(device, "vkDestroyCommandPool");
        vkDestroyRenderPass = (PFN_vkDestroyRenderPass)vkGetDeviceProcAddr(device, "vkDestroyRenderPass");
        vkDestroyFramebuffer = (PFN_vkDestroyFramebuffer)vkGetDeviceProcAddr(device, "vkDestroyFramebuffer");
        vkDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)vkGetDeviceProcAddr(device, "vkDestroySwapchainKHR");
        vkDestroyShaderModule = (PFN_vkDestroyShaderModule)vkGetDeviceProcAddr(device, "vkDestroyShaderModule");

        return true;
    }

    void cleanup() { libraryLoader.cleanup(); }

    [[nodiscard]] bool enumerateInstanceExtensionProperties(char const* pLayerName, std::uint32_t* pPropertyCount, VkExtensionProperties* pProperties) const
    {
        return !checkError(vkEnumerateInstanceExtensionProperties(pLayerName, pPropertyCount, pProperties), ""sv,
                           "Failed to enumerate available instance extensions"sv);
    }
    [[nodiscard]] bool enumerateInstanceLayerProperties(std::uint32_t* pPropertyCount, VkLayerProperties* pProperties) const
    {
        return !checkError(vkEnumerateInstanceLayerProperties(pPropertyCount, pProperties), ""sv, "Failed to enumerate available instance layer properties"sv);
    }
    [[nodiscard]] bool createInstance(VkInstanceCreateInfo const* pCreateInfo, VkAllocationCallbacks const* pAllocator, VkInstance* pInstance) const
    {
        return !checkError(vkCreateInstance(pCreateInfo, pAllocator, pInstance), "Created Vulkan instance"sv, "Failed to create Vulkan instance"sv);
    }
    [[nodiscard]] bool createDevice(VkPhysicalDevice physicalDevice, VkDeviceCreateInfo const* pCreateInfo, VkAllocationCallbacks const* pAllocator,
                                    VkDevice* pDevice) const
    {
        return !checkError(vkCreateDevice(physicalDevice, pCreateInfo, pAllocator, pDevice), "Created device"sv, "Failed to create device"sv);
    }
    [[nodiscard]] bool createSwapchain(VkDevice device, VkSwapchainCreateInfoKHR const* pCreateInfo, VkAllocationCallbacks const* pAllocator,
                                       VkSwapchainKHR* pSwapchain) const
    {
        return !checkError(vkCreateSwapchainKHR(device, pCreateInfo, pAllocator, pSwapchain), "Created swapchain"sv, "Failed to create swapchain"sv);
    }
    [[nodiscard]] bool createImageView(VkDevice device, VkImageViewCreateInfo const* pCreateInfo, VkAllocationCallbacks const* pAllocator,
                                       VkImageView* pView) const
    {
        return !checkError(vkCreateImageView(device, pCreateInfo, pAllocator, pView), ""sv, "Failed to create image view"sv);
    }
    [[nodiscard]] bool createRenderPass(VkDevice device, VkRenderPassCreateInfo const* pCreateInfo, VkAllocationCallbacks const* pAllocator,
                                        VkRenderPass* pRenderPass) const
    {
        return !checkError(vkCreateRenderPass(device, pCreateInfo, pAllocator, pRenderPass), "Created render pass"sv, "Failed to create render pass"sv);
    }
    [[nodiscard]] bool createFramebuffer(VkDevice device, const VkFramebufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator,
                                         VkFramebuffer* pFramebuffer) const
    {
        return !checkError(vkCreateFramebuffer(device, pCreateInfo, pAllocator, pFramebuffer), ""sv, "Failed to create framebuffer"sv);
    }
    [[nodiscard]] bool createShaderModule(VkDevice device, VkShaderModuleCreateInfo const* pCreateInfo, VkAllocationCallbacks const* pAllocator,
                                          VkShaderModule* pShaderModule) const
    {
        return !checkError(vkCreateShaderModule(device, pCreateInfo, pAllocator, pShaderModule), ""sv, "Failed to create shader module"sv);
    }
    [[nodiscard]] bool createBuffer(VkDevice device, VkBufferCreateInfo const* pCreateInfo, VkAllocationCallbacks const* pAllocator, VkBuffer* pBuffer) const
    {
        return !checkError(vkCreateBuffer(device, pCreateInfo, pAllocator, pBuffer), ""sv, "Failed to create buffer"sv);
    }
    [[nodiscard]] bool createSemaphore(VkDevice device, VkSemaphoreCreateInfo const* pCreateInfo, VkAllocationCallbacks const* pAllocator,
                                       VkSemaphore* pSemaphore) const
    {
        return !checkError(vkCreateSemaphore(device, pCreateInfo, pAllocator, pSemaphore), ""sv, "Failed to create semaphore"sv);
    }
    [[nodiscard]] bool createFence(VkDevice device, VkFenceCreateInfo const* pCreateInfo, VkAllocationCallbacks const* pAllocator, VkFence* pFence) const
    {
        return !checkError(vkCreateFence(device, pCreateInfo, pAllocator, pFence), ""sv, "Failed to create fence"sv);
    }
    [[nodiscard]] bool createCommandPool(VkDevice device, VkCommandPoolCreateInfo const* pCreateInfo, VkAllocationCallbacks const* pAllocator,
                                         VkCommandPool* pCommandPool) const
    {
        return !checkError(vkCreateCommandPool(device, pCreateInfo, pAllocator, pCommandPool), ""sv, "Failed to create command pool"sv);
    }
    [[nodiscard]] bool createImage(VkDevice device, VkImageCreateInfo const* pCreateInfo, VkAllocationCallbacks const* pAllocator, VkImage* pImage) const
    {
        return !checkError(vkCreateImage(device, pCreateInfo, pAllocator, pImage), ""sv, "Failed to create image"sv);
    }
    [[nodiscard]] bool createSampler(VkDevice device, VkSamplerCreateInfo const* pCreateInfo, VkAllocationCallbacks const* pAllocator,
                                     VkSampler* pSampler) const
    {
        return !checkError(vkCreateSampler(device, pCreateInfo, pAllocator, pSampler), ""sv, "Failed to create sampler"sv);
    }
    [[nodiscard]] bool createDescriptorSetLayout(VkDevice device, VkDescriptorSetLayoutCreateInfo const* pCreateInfo, VkAllocationCallbacks const* pAllocator,
                                                 VkDescriptorSetLayout* pSetLayout) const
    {
        return !checkError(vkCreateDescriptorSetLayout(device, pCreateInfo, pAllocator, pSetLayout), ""sv, "Failed to create descriptor set layout"sv);
    }
    [[nodiscard]] bool createDescriptorPool(VkDevice device, VkDescriptorPoolCreateInfo const* pCreateInfo, VkAllocationCallbacks const* pAllocator,
                                            VkDescriptorPool* pDescriptorPool) const
    {
        return !checkError(vkCreateDescriptorPool(device, pCreateInfo, pAllocator, pDescriptorPool), ""sv, "Failed to create descriptor pool"sv);
    }
    [[nodiscard]] bool createPipelineLayout(VkDevice device, VkPipelineLayoutCreateInfo const* pCreateInfo, VkAllocationCallbacks const* pAllocator,
                                            VkPipelineLayout* pPipelineLayout) const
    {
        return !checkError(vkCreatePipelineLayout(device, pCreateInfo, pAllocator, pPipelineLayout), ""sv, "Failed to create pipeline layout"sv);
    }
    [[nodiscard]] bool createGraphicsPipelines(VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount,
                                               VkGraphicsPipelineCreateInfo const* pCreateInfos, VkAllocationCallbacks const* pAllocator,
                                               VkPipeline* pPipelines) const
    {
        return !checkError(vkCreateGraphicsPipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines), ""sv,
                           "Failed to create graphics pipelines"sv);
    }

    [[nodiscard]] bool createDebugUtilsMessenger(VkInstance instance, VkDebugUtilsMessengerCreateInfoEXT const* pCreateInfo,
                                                 VkAllocationCallbacks const* pAllocator, VkDebugUtilsMessengerEXT* pMessenger) const
    {
        return !checkError(vkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger), "Created debug messenger"sv,
                           "Failed to create debug messenger"sv);
    }
    [[nodiscard]] bool enumeratePhysicalDevices(VkInstance instance, std::uint32_t* pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices) const
    {
        return !checkError(vkEnumeratePhysicalDevices(instance, pPhysicalDeviceCount, pPhysicalDevices), ""sv, "Failed to enumerate physical devices"sv);
    }
    void getPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties* pProperties) const
    {
        vkGetPhysicalDeviceProperties(physicalDevice, pProperties);
    }
    void getPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice, std::uint32_t* pQueueFamilyPropertyCount,
                                                VkQueueFamilyProperties* pQueueFamilyProperties) const
    {
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
    }
    [[nodiscard]] bool getPhysicalDeviceSurfaceSupport(VkPhysicalDevice physicalDevice, std::uint32_t queueFamilyIndex, VkSurfaceKHR surface,
                                                       VkBool32* pSupported) const
    {
        return !checkError(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, pSupported), ""sv,
                           "Failed to get physical device surface support"sv);
    }
    [[nodiscard]] bool enumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice, char const* pLayerName, std::uint32_t* pPropertyCount,
                                                          VkExtensionProperties* pProperties) const
    {
        return !checkError(vkEnumerateDeviceExtensionProperties(physicalDevice, pLayerName, pPropertyCount, pProperties), ""sv,
                           "Failed to enumerate physical device extension properties"sv);
    }
    [[nodiscard]] bool getPhysicalDeviceSurfaceFormats(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, std::uint32_t* pSurfaceFormatCount,
                                                       VkSurfaceFormatKHR* pSurfaceFormats) const
    {
        return !checkError(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, pSurfaceFormatCount, pSurfaceFormats), ""sv,
                           "Failed to get physical device surface formats"sv);
    }
    [[nodiscard]] bool getPhysicalDeviceSurfacePresentModes(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, std::uint32_t* pPresentModeCount,
                                                            VkPresentModeKHR* pPresentModes) const
    {
        return !checkError(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, pPresentModeCount, pPresentModes), ""sv,
                           "Failed to get physical device surface present modes"sv);
    }
    [[nodiscard]] bool getPhysicalDeviceSurfaceCapabilities(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                                                            VkSurfaceCapabilitiesKHR* pSurfaceCapabilities) const
    {
        return !checkError(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, pSurfaceCapabilities), ""sv,
                           "Failed to get physical device surface capabilities"sv);
    }
    void getPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties* pMemoryProperties) const
    {
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, pMemoryProperties);
    }
    void getPhysicalDeviceFeatures2(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures2* pFeatures) const
    {
        vkGetPhysicalDeviceFeatures2(physicalDevice, pFeatures);
    }

    [[nodiscard]] bool getSwapchainImages(VkDevice device, VkSwapchainKHR swapchain, std::uint32_t* pSwapchainImageCount, VkImage* pSwapchainImages) const
    {
        return !checkError(vkGetSwapchainImagesKHR(device, swapchain, pSwapchainImageCount, pSwapchainImages), ""sv, "Failed to get swapchain images"sv);
    }
    [[nodiscard]] bool bindBufferMemory(VkDevice device, VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset) const
    {
        return !checkError(vkBindBufferMemory(device, buffer, memory, memoryOffset), ""sv, "Failed to bind buffer memory"sv);
    }
    [[nodiscard]] bool bindImageMemory(VkDevice device, VkImage image, VkDeviceMemory memory, VkDeviceSize memoryOffset) const
    {
        return !checkError(vkBindImageMemory(device, image, memory, memoryOffset), ""sv, "Failed to bind image memory"sv);
    }
    [[nodiscard]] bool allocateCommandBuffers(VkDevice device, VkCommandBufferAllocateInfo const* pAllocateInfo, VkCommandBuffer* pCommandBuffers) const
    {
        return !checkError(vkAllocateCommandBuffers(device, pAllocateInfo, pCommandBuffers), ""sv, "Failed to allocate command buffers"sv);
    }
    [[nodiscard]] bool beginCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferBeginInfo const* pBeginInfo) const
    {
        return !checkError(vkBeginCommandBuffer(commandBuffer, pBeginInfo), ""sv, "Failed to begin command buffer"sv);
    }
    [[nodiscard]] bool endCommandBuffer(VkCommandBuffer commandBuffer) const
    {
        return !checkError(vkEndCommandBuffer(commandBuffer), ""sv, "Failed to end command buffer"sv);
    }
    [[nodiscard]] bool resetCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags) const
    {
        return !checkError(vkResetCommandBuffer(commandBuffer, flags), ""sv, "Failed to reset command buffer"sv);
    }
    [[nodiscard]] bool queueSubmit(VkQueue queue, std::uint32_t submitCount, VkSubmitInfo const* pSubmits, VkFence fence) const
    {
        return !checkError(vkQueueSubmit(queue, submitCount, pSubmits, fence), ""sv, "Failed to submit queue"sv);
    }
    [[nodiscard]] VkResult queuePresent(VkQueue queue, VkPresentInfoKHR const* pPresentInfo) const { return vkQueuePresentKHR(queue, pPresentInfo); }
    [[nodiscard]] bool queueWaitIdle(VkQueue queue) const { return !checkError(vkQueueWaitIdle(queue), ""sv, "Failed to wait for idle queue"sv); }
    [[nodiscard]] bool allocateDescriptorSets(VkDevice device, VkDescriptorSetAllocateInfo const* pAllocateInfo, VkDescriptorSet* pDescriptorSets) const
    {
        return !checkError(vkAllocateDescriptorSets(device, pAllocateInfo, pDescriptorSets), ""sv, "Failed to allocate descriptor sets"sv);
    }
    [[nodiscard]] bool waitForFences(VkDevice device, uint32_t fenceCount, VkFence const* pFences, VkBool32 waitAll, uint64_t timeout) const
    {
        return !checkError(vkWaitForFences(device, fenceCount, pFences, waitAll, timeout), ""sv, "Failed to wait for fences"sv);
    }
    [[nodiscard]] bool resetFences(VkDevice device, uint32_t fenceCount, VkFence const* pFences) const
    {
        return !checkError(vkResetFences(device, fenceCount, pFences), ""sv, "Failed to reset fences"sv);
    }
    [[nodiscard]] VkResult acquireNextImage(VkDevice device, VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence,
                                            uint32_t* pImageIndex) const
    {
        return vkAcquireNextImageKHR(device, swapchain, timeout, semaphore, fence, pImageIndex);
    }
    [[nodiscard]] bool deviceWaitIdle(VkDevice device) const { return !checkError(vkDeviceWaitIdle(device), ""sv, "Failed to wait for device idle"sv); }
    [[nodiscard]] bool allocateMemory(VkDevice device, VkMemoryAllocateInfo const* pAllocateInfo, VkAllocationCallbacks const* pAllocator,
                                      VkDeviceMemory* pMemory) const
    {
        return !checkError(vkAllocateMemory(device, pAllocateInfo, pAllocator, pMemory), ""sv, "Failed to allocate memory"sv);
    }
    [[nodiscard]] bool mapMemory(VkDevice device, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void** ppData) const
    {
        return !checkError(vkMapMemory(device, memory, offset, size, flags, ppData), ""sv, "Failed to map memory"sv);
    }
    void getDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue* pQueue) const
    {
        vkGetDeviceQueue(device, queueFamilyIndex, queueIndex, pQueue);
    }
    void getImageMemoryRequirements(VkDevice device, VkImage image, VkMemoryRequirements* pMemoryRequirements) const
    {
        vkGetImageMemoryRequirements(device, image, pMemoryRequirements);
    }
    void getBufferMemoryRequirements(VkDevice device, VkBuffer buffer, VkMemoryRequirements* pMemoryRequirements) const
    {
        vkGetBufferMemoryRequirements(device, buffer, pMemoryRequirements);
    }
    VkDeviceAddress getBufferDeviceAddress(VkDevice device, VkBufferDeviceAddressInfo const* pInfo) const { return vkGetBufferDeviceAddress(device, pInfo); }
    void updateDescriptorSets(VkDevice device, uint32_t descriptorWriteCount, VkWriteDescriptorSet const* pDescriptorWrites, uint32_t descriptorCopyCount,
                              VkCopyDescriptorSet const* pDescriptorCopies) const
    {
        vkUpdateDescriptorSets(device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
    }

    void commandPipelineBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
                                VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount, VkMemoryBarrier const* pMemoryBarriers,
                                uint32_t bufferMemoryBarrierCount, VkBufferMemoryBarrier const* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount,
                                VkImageMemoryBarrier const* pImageMemoryBarriers) const
    {
        vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount,
                             pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
    }
    void commandCopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount,
                                  VkBufferImageCopy const* pRegions) const
    {
        vkCmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
    }
    void commandBeginRenderPass(VkCommandBuffer commandBuffer, VkRenderPassBeginInfo const* pRenderPassBegin, VkSubpassContents contents) const
    {
        vkCmdBeginRenderPass(commandBuffer, pRenderPassBegin, contents);
    }
    void commandBindPipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline) const
    {
        vkCmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
    }
    void commandSetViewport(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, VkViewport const* pViewports) const
    {
        vkCmdSetViewport(commandBuffer, firstViewport, viewportCount, pViewports);
    }
    void commandSetScissor(VkCommandBuffer commandBuffer, uint32_t firstScissor, uint32_t scissorCount, VkRect2D const* pScissors) const
    {
        vkCmdSetScissor(commandBuffer, firstScissor, scissorCount, pScissors);
    }
    void commandPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout layout, VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size,
                              void const* pValues) const
    {
        vkCmdPushConstants(commandBuffer, layout, stageFlags, offset, size, pValues);
    }
    void commandBindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet,
                                   uint32_t descriptorSetCount, VkDescriptorSet const* pDescriptorSets, uint32_t dynamicOffsetCount,
                                   uint32_t const* pDynamicOffsets) const
    {
        vkCmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
    }
    void commandDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) const
    {
        vkCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
    }
    void commandEndRenderPass(VkCommandBuffer commandBuffer) const { vkCmdEndRenderPass(commandBuffer); }

    void freeCommandBuffers(VkDevice device, VkCommandPool commandPool, std::uint32_t commandBufferCount, VkCommandBuffer const* pCommandBuffers) const
    {
        vkFreeCommandBuffers(device, commandPool, commandBufferCount, pCommandBuffers);
    }
    void destroyDebugUtilsMessenger(VkInstance instance, VkDebugUtilsMessengerEXT messenger, VkAllocationCallbacks const* pAllocator) const
    {
        vkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
    }
    void destroyInstance(VkInstance instance, VkAllocationCallbacks const* pAllocator) const { vkDestroyInstance(instance, pAllocator); }
    void freeMemory(VkDevice device, VkDeviceMemory memory, VkAllocationCallbacks const* pAllocator) const { vkFreeMemory(device, memory, pAllocator); }
    void destroyBuffer(VkDevice device, VkBuffer buffer, VkAllocationCallbacks const* pAllocator) const { vkDestroyBuffer(device, buffer, pAllocator); }
    void destroyPipeline(VkDevice device, VkPipeline pipeline, VkAllocationCallbacks const* pAllocator) const
    {
        vkDestroyPipeline(device, pipeline, pAllocator);
    }
    void destroyPipelineLayout(VkDevice device, VkPipelineLayout pipelineLayout, VkAllocationCallbacks const* pAllocator) const
    {
        vkDestroyPipelineLayout(device, pipelineLayout, pAllocator);
    }
    void destroyDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, VkAllocationCallbacks const* pAllocator) const
    {
        vkDestroyDescriptorPool(device, descriptorPool, pAllocator);
    }
    void destroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, VkAllocationCallbacks const* pAllocator) const
    {
        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, pAllocator);
    }
    void destroySampler(VkDevice device, VkSampler sampler, VkAllocationCallbacks const* pAllocator) const { vkDestroySampler(device, sampler, pAllocator); }
    void destroyImageView(VkDevice device, VkImageView imageView, VkAllocationCallbacks const* pAllocator) const
    {
        vkDestroyImageView(device, imageView, pAllocator);
    }
    void destroyImage(VkDevice device, VkImage image, VkAllocationCallbacks const* pAllocator) const { vkDestroyImage(device, image, pAllocator); }
    void destroyCommandPool(VkDevice device, VkCommandPool commandPool, VkAllocationCallbacks const* pAllocator) const
    {
        vkDestroyCommandPool(device, commandPool, pAllocator);
    }
    void destroyDevice(VkDevice device, VkAllocationCallbacks const* pAllocator) const { vkDestroyDevice(device, pAllocator); }
    void destroyRenderPass(VkDevice device, VkRenderPass renderPass, VkAllocationCallbacks const* pAllocator) const
    {
        vkDestroyRenderPass(device, renderPass, pAllocator);
    }
    void destroyFramebuffer(VkDevice device, VkFramebuffer framebuffer, VkAllocationCallbacks const* pAllocator) const
    {
        vkDestroyFramebuffer(device, framebuffer, pAllocator);
    }
    void destroySwapchain(VkDevice device, VkSwapchainKHR swapchain, VkAllocationCallbacks const* pAllocator) const
    {
        vkDestroySwapchainKHR(device, swapchain, pAllocator);
    }
    void destroyShaderModule(VkDevice device, VkShaderModule shaderModule, VkAllocationCallbacks const* pAllocator) const
    {
        vkDestroyShaderModule(device, shaderModule, pAllocator);
    }
    void unmapMemory(VkDevice device, VkDeviceMemory memory) const { vkUnmapMemory(device, memory); }
    void destroySemaphore(VkDevice device, VkSemaphore semaphore, VkAllocationCallbacks const* pAllocator) const
    {
        vkDestroySemaphore(device, semaphore, pAllocator);
    }
    void destroyFence(VkDevice device, VkFence fence, VkAllocationCallbacks const* pAllocator) const { vkDestroyFence(device, fence, pAllocator); }

private:
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr{};

    PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties{};
    PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties{};
    PFN_vkCreateInstance vkCreateInstance{};

    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT{};
    PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices{};
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT{};
    PFN_vkDestroyInstance vkDestroyInstance{};

    PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties{};
    PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties{};
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR{};
    PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties{};
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR{};
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR{};
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR{};
    PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties{};
    PFN_vkGetPhysicalDeviceFeatures2 vkGetPhysicalDeviceFeatures2{};
    PFN_vkCreateDevice vkCreateDevice{};

    PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR{};
    PFN_vkCreateImageView vkCreateImageView{};
    PFN_vkCreateRenderPass vkCreateRenderPass{};
    PFN_vkCreateFramebuffer vkCreateFramebuffer{};
    PFN_vkCreateShaderModule vkCreateShaderModule{};
    PFN_vkCreateBuffer vkCreateBuffer{};
    PFN_vkCreateSemaphore vkCreateSemaphore{};
    PFN_vkCreateFence vkCreateFence{};
    PFN_vkCreateCommandPool vkCreateCommandPool{};
    PFN_vkCreateImage vkCreateImage{};
    PFN_vkCreateSampler vkCreateSampler{};
    PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout{};
    PFN_vkCreateDescriptorPool vkCreateDescriptorPool{};
    PFN_vkCreatePipelineLayout vkCreatePipelineLayout{};
    PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines{};
    PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr{};
    PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR{};
    PFN_vkBindBufferMemory vkBindBufferMemory{};
    PFN_vkBindImageMemory vkBindImageMemory{};
    PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers{};
    PFN_vkBeginCommandBuffer vkBeginCommandBuffer{};
    PFN_vkEndCommandBuffer vkEndCommandBuffer{};
    PFN_vkResetCommandBuffer vkResetCommandBuffer{};
    PFN_vkQueueSubmit vkQueueSubmit{};
    PFN_vkQueueWaitIdle vkQueueWaitIdle{};
    PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets{};
    PFN_vkWaitForFences vkWaitForFences{};
    PFN_vkResetFences vkResetFences{};
    PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR{};
    PFN_vkQueuePresentKHR vkQueuePresentKHR{};
    PFN_vkDeviceWaitIdle vkDeviceWaitIdle{};
    PFN_vkAllocateMemory vkAllocateMemory{};
    PFN_vkMapMemory vkMapMemory{};
    PFN_vkGetDeviceQueue vkGetDeviceQueue{};
    PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements{};
    PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements{};
    PFN_vkGetBufferDeviceAddress vkGetBufferDeviceAddress{};
    PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets{};
    PFN_vkFreeCommandBuffers vkFreeCommandBuffers{};
    PFN_vkFreeMemory vkFreeMemory{};
    PFN_vkDestroyBuffer vkDestroyBuffer{};
    PFN_vkDestroyPipeline vkDestroyPipeline{};
    PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout{};
    PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool{};
    PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout{};
    PFN_vkDestroySampler vkDestroySampler{};
    PFN_vkDestroyImageView vkDestroyImageView{};
    PFN_vkDestroyImage vkDestroyImage{};
    PFN_vkDestroyCommandPool vkDestroyCommandPool{};
    PFN_vkDestroyDevice vkDestroyDevice{};
    PFN_vkDestroyRenderPass vkDestroyRenderPass{};
    PFN_vkDestroyFramebuffer vkDestroyFramebuffer{};
    PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR{};
    PFN_vkDestroyShaderModule vkDestroyShaderModule{};
    PFN_vkUnmapMemory vkUnmapMemory{};
    PFN_vkDestroySemaphore vkDestroySemaphore{};
    PFN_vkDestroyFence vkDestroyFence{};

    PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier{};
    PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage{};
    PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass{};
    PFN_vkCmdBindPipeline vkCmdBindPipeline{};
    PFN_vkCmdSetViewport vkCmdSetViewport{};
    PFN_vkCmdSetScissor vkCmdSetScissor{};
    PFN_vkCmdPushConstants vkCmdPushConstants{};
    PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets{};
    PFN_vkCmdDraw vkCmdDraw{};
    PFN_vkCmdEndRenderPass vkCmdEndRenderPass{};

    LibraryLoader libraryLoader;
};
inline VulkanLibrary lib;

/*
 * RenderWindow - class for handling SDL initialization and window creation
 */
export class RenderWindow
{
public:
    enum class State
    {
        eWindowed,
        eMaximized,
        eFullscreen
    };

    RenderWindow() = default;
    // Initialize SDL and create a window
    [[nodiscard]] bool init(std::string_view name, std::string_view version, std::string_view identifier, std::string_view creator, std::string_view copyright,
                            std::string_view type, std::int64_t width, std::int64_t height, State state)
    {
        // Set application metadata
        FixedString<256> fullAppName;
        fullAppName.format("{} {}", name, version);
        if (checkSDLError(SDL_SetAppMetadataProperty(sdlPropAppMetadataNameString, fullAppName.getData()) &&
                          SDL_SetAppMetadataProperty(sdlPropAppMetadataVersionString, version.data()) &&
                          SDL_SetAppMetadataProperty(sdlPropAppMetadataIdentifierString, identifier.data()) &&
                          SDL_SetAppMetadataProperty(sdlPropAppMetadataCreatorString, creator.data()) &&
                          SDL_SetAppMetadataProperty(sdlPropAppMetadataCopyrightString, copyright.data()) &&
                          SDL_SetAppMetadataProperty(sdlPropAppMetadataTypeString, type.data())))
            return false;

        // Initialize SDL video system
        if (checkSDLError(SDL_Init(sdlInitVideo)))
            return false;

        int displayCount{};
        auto displays = SDL_GetDisplays(&displayCount);
        if (checkSDLError(displays))
            return false;

        SDL_Rect displayUsableRect{};
        if (checkSDLError(SDL_GetDisplayUsableBounds(displays[0], &displayUsableRect)))
            return false;
        SDL_free(displays);

        auto windowWidth = std::min((std::int64_t)displayUsableRect.w, width);
        auto windowHeight = std::min((std::int64_t)displayUsableRect.h, height);

        // Create window
        window = SDL_CreateWindow(fullAppName.getData(), windowWidth, windowHeight,
                                  sdlWindowVulkan | sdlWindowResizable |
                                      (state == State::eFullscreen      ? sdlWindowFullscreen
                                           : state == State::eMaximized ? sdlWindowMaximized
                                                                        : 0));
        if (checkSDLError(window))
            return false;

        auto usableCenterX = displayUsableRect.x + displayUsableRect.w / 2;
        auto usableCenterY = displayUsableRect.y + displayUsableRect.h / 2;
        if (auto topLeftX = usableCenterX - windowWidth / 2, topLeftY = usableCenterY - windowHeight / 2;
            checkSDLError(SDL_SetWindowPosition(window, topLeftX, topLeftY)))
            return false;

        if (checkSDLError(SDL_SyncWindow(window)))
            return false;

        // Get required extensions
        std::uint32_t extensionCount{};
        auto vulkanExtensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
        if (checkSDLError(vulkanExtensions))
            return false;

        requiredExtensions = {vulkanExtensions, extensionCount};

        return true;
    }
    void cleanup()
    {
        if (window)
            SDL_DestroyWindow(window);

        if (SDL_WasInit(sdlInitVideo))
            SDL_QuitSubSystem(sdlInitVideo);

        SDL_Quit();
    }

    // Create Vulkan surface from window
    [[nodiscard]] bool createSurface(VkInstance instance, VkAllocationCallbacks const* allocator, VkSurfaceKHR* surface) const
    {
        return !checkSDLError(SDL_Vulkan_CreateSurface(window, instance, allocator, surface));
    }
    void destroySurface(VkInstance instance, VkSurfaceKHR surface, VkAllocationCallbacks const* allocator) const
    {
        SDL_Vulkan_DestroySurface(instance, surface, allocator);
    }

    // Get the window size in pixels
    [[nodiscard]] std::pair<std::int64_t, std::int64_t> getWindowSize() const
    {
        int width{}, height{};
        SDL_GetWindowSizeInPixels(window, &width, &height);
        return {width, height};
    }

    [[nodiscard]] auto getRequiredExtensions() const { return requiredExtensions; }
    [[nodiscard]] bool getIsMaximized() const { return SDL_GetWindowFlags(window) & sdlWindowMaximized; }
    [[nodiscard]] bool getIsFullscreen() const { return SDL_GetWindowFlags(window) & sdlWindowFullscreen; }

    // Maximize the window and sync changes
    void setIsMaximized(bool maximized) const
    {
        maximized ? SDL_MaximizeWindow(window) : SDL_RestoreWindow(window);
        SDL_SyncWindow(window);
    }
    // Fullscreen the window and sync changes
    void setIsFullscreen(bool fullscreen) const
    {
        SDL_SetWindowFullscreen(window, fullscreen);
        SDL_SyncWindow(window);
    }

private:
    [[nodiscard]] static bool checkSDLError(bool value)
    {
        if (value)
            return false;

        if (auto errorString = SDL_GetError(); errorString[0] != '\0')
        {
            logger.logError("{}", errorString);
            SDL_ClearError();
        }

        return true;
    }

    SDL_Window* window{};
    std::span<char const* const> requiredExtensions{};
};
export inline RenderWindow renderWindow;

/*
 * Class for handling Vulkan resources
 */
export class RenderEngine
{
    static constexpr std::uint32_t maxFramesInFlight{2};

    // Struct for storing physical device properties
    struct PhysicalDeviceInfo
    {
        [[nodiscard]] std::int32_t getMemoryTypeIndex(VkMemoryRequirements const& requirements, VkMemoryPropertyFlags properties) const
        {
            std::int32_t selectedMemoryType{-1};
            for (std::uint32_t i{}; i < memoryProperties.memoryTypeCount; i++)
            {
                if (requirements.memoryTypeBits & 1 << i && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
                {
                    selectedMemoryType = i;
                    break;
                }
            }
            if (selectedMemoryType == -1)
                logger.logError("Failed to find suitable memory type for buffer");

            return selectedMemoryType;
        }

        FixedString<256> name;
        std::uint32_t surfaceFormatCount{};
        Array<VkSurfaceFormatKHR, 16> surfaceFormats;
        std::uint32_t presentModeCount{};
        Array<VkPresentModeKHR, 16> presentModes;
        VkSurfaceCapabilitiesKHR surfaceCapabilities{};
        std::uint32_t graphicsIndex{}, presentationIndex{};
        float maxSamplerAnisotropy{};
        VkPhysicalDeviceMemoryProperties memoryProperties{};
    };

    // Struct for storing swapchain resources
    struct SwapchainResources
    {
        SwapchainResources() = default;

        [[nodiscard]] bool init(VulkanLibrary const& lib, PhysicalDeviceInfo const& physicalDeviceInfo, VkDevice device, VkSurfaceKHR surface,
                                VkSwapchainKHR oldSwapchain)
        {
            // Choose surface format
            VkSurfaceFormatKHR selectedFormat{physicalDeviceInfo.surfaceFormats[0]};
            for (auto format : physicalDeviceInfo.surfaceFormats)
            {
                if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                    selectedFormat = format;
            }
            imageFormat = selectedFormat.format;
            logger.logInfo("Chose format {} with color space {}", enumToStr(selectedFormat.format), enumToStr(selectedFormat.colorSpace));

            // Choose present mode
            auto selectedPresentMode{VK_PRESENT_MODE_FIFO_KHR};
            for (auto presentMode : physicalDeviceInfo.presentModes)
            {
                if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                    selectedPresentMode = presentMode;
            }
            logger.logInfo("Chose present mode {}", enumToStr(selectedPresentMode));

            // Choose swapchain extent
            auto const& surfaceCapabilities = physicalDeviceInfo.surfaceCapabilities;
            imageExtent = surfaceCapabilities.currentExtent;
            if (imageExtent.width == std::numeric_limits<std::uint32_t>::max())
            {
                auto [framebufferWidth, framebufferHeight] = renderWindow.getWindowSize();
                imageExtent.width =
                    std::clamp((std::uint32_t)framebufferWidth, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
                imageExtent.height =
                    std::clamp((std::uint32_t)framebufferHeight, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
            }
            logger.logInfo("Swap extent is [{},{}]", imageExtent.width, imageExtent.height);

            // Create swapchain
            VkSharingMode sharingMode{physicalDeviceInfo.graphicsIndex != physicalDeviceInfo.presentationIndex ? VK_SHARING_MODE_CONCURRENT
                                                                                                               : VK_SHARING_MODE_EXCLUSIVE};
            FixedVector<std::uint32_t, 2> queueFamilyIndices;
            if (sharingMode == VK_SHARING_MODE_CONCURRENT)
            {
                queueFamilyIndices.emplaceBack(physicalDeviceInfo.graphicsIndex);
                queueFamilyIndices.emplaceBack(physicalDeviceInfo.presentationIndex);
            }

            VkSwapchainCreateInfoKHR swapchainCreateInfo{.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                                                         .pNext = nullptr,
                                                         .flags = {},
                                                         .surface = surface,
                                                         .minImageCount = std::max(surfaceCapabilities.minImageCount, maxFramesInFlight),
                                                         .imageFormat = selectedFormat.format,
                                                         .imageColorSpace = selectedFormat.colorSpace,
                                                         .imageExtent = imageExtent,
                                                         .imageArrayLayers = 1,
                                                         .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                                         .imageSharingMode = sharingMode,
                                                         .queueFamilyIndexCount = (std::uint32_t)queueFamilyIndices.getSize(),
                                                         .pQueueFamilyIndices = queueFamilyIndices.getData(),
                                                         .preTransform = surfaceCapabilities.currentTransform,
                                                         .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                                                         .presentMode = selectedPresentMode,
                                                         .clipped = VK_TRUE,
                                                         .oldSwapchain = oldSwapchain};
            if (!lib.createSwapchain(device, &swapchainCreateInfo, nullptr, &swapchain))
                return false;

            // Get swapchain images
            imageCount = images.getSize();
            if (!lib.getSwapchainImages(device, swapchain, &imageCount, images.getData()))
                return false;
            logger.logInfo("Image count is {}", imageCount);

            // Create swapchain image views
            for (std::uint32_t i{}; i < imageCount; i++)
            {
                VkImageSubresourceRange subresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
                VkImageViewCreateInfo viewCreateInfo{
                    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = {},
                    .image = images[i],
                    .viewType = VK_IMAGE_VIEW_TYPE_2D,
                    .format = imageFormat,
                    .components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
                    .subresourceRange = subresourceRange};
                if (!lib.createImageView(device, &viewCreateInfo, nullptr, &imageViews[i]))
                    return false;
            }

            // Define attachment
            VkAttachmentDescription colorAttachment{.flags = {},
                                                    .format = imageFormat,
                                                    .samples = VK_SAMPLE_COUNT_1_BIT,
                                                    .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                                    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                                    .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                                    .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                                    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                                    .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR};
            VkAttachmentReference colorAttachmentReference{0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};

            // Create render pass
            VkSubpassDescription subpassDescription{.flags = {},
                                                    .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
                                                    .inputAttachmentCount = 0,
                                                    .pInputAttachments = nullptr,
                                                    .colorAttachmentCount = 1,
                                                    .pColorAttachments = &colorAttachmentReference,
                                                    .pResolveAttachments = nullptr,
                                                    .pDepthStencilAttachment = nullptr,
                                                    .preserveAttachmentCount = 0,
                                                    .pPreserveAttachments = nullptr};
            VkSubpassDependency subpassDependency{VK_SUBPASS_EXTERNAL,
                                                  0,
                                                  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                                  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                                  {},
                                                  VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                                  {}};
            VkRenderPassCreateInfo renderPassCreateInfo{.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                                                        .pNext = nullptr,
                                                        .flags = {},
                                                        .attachmentCount = 1,
                                                        .pAttachments = &colorAttachment,
                                                        .subpassCount = 1,
                                                        .pSubpasses = &subpassDescription,
                                                        .dependencyCount = 1,
                                                        .pDependencies = &subpassDependency};
            if (!lib.createRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass))
                return false;

            // Create swapchain framebuffers
            for (std::uint32_t i{}; i < imageCount; i++)
            {
                VkFramebufferCreateInfo framebufferCreateInfo{.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                                                              .pNext = nullptr,
                                                              .flags = {},
                                                              .renderPass = renderPass,
                                                              .attachmentCount = 1,
                                                              .pAttachments = &imageViews[i],
                                                              .width = imageExtent.width,
                                                              .height = imageExtent.height,
                                                              .layers = 1};
                if (!lib.createFramebuffer(device, &framebufferCreateInfo, nullptr, &framebuffers[i]))
                    return false;
            }
            logger.logInfo("Created swapchain framebuffers");

            return true;
        }

        void cleanup(VulkanLibrary const& lib, VkDevice device)
        {
            lib.destroyRenderPass(device, renderPass, nullptr);

            for (std::uint32_t i{}; i < imageCount; i++)
            {
                lib.destroyFramebuffer(device, framebuffers[i], nullptr);
                lib.destroyImageView(device, imageViews[i], nullptr);
            }

            lib.destroySwapchain(device, swapchain, nullptr);
        }

        VkSwapchainKHR swapchain{};
        std::uint32_t imageCount{};
        Array<VkImage, 16> images;
        VkFormat imageFormat{};
        VkExtent2D imageExtent{};
        Array<VkImageView, 16> imageViews;
        VkRenderPass renderPass{};
        Array<VkFramebuffer, 16> framebuffers;
    };

    // Struct for storing buffer resources
    struct BufferResources
    {
        VkBuffer buffer{};
        VkDeviceMemory bufferMemory{};
        VkDeviceAddress bufferAddress{};
        void* data{};
    };

    class ShaderModule
    {
    public:
        ShaderModule() = default;
        [[nodiscard]] bool init(VkDevice device, std::string_view fileName)
        {
            std::ifstream shaderFile(fileName.data(), std::ios::ate | std::ios::binary | std::ios::in);

            if (!shaderFile)
            {
                logger.logError("Failed to open shader file {}", fileName.data());
                return false;
            }

            auto fileSize{shaderFile.tellg()};
            Array<std::uint32_t, 4096> fileBuffer;
            shaderFile.seekg(0);
            shaderFile.read(reinterpret_cast<char*>(fileBuffer.getData()), fileSize);

            VkShaderModuleCreateInfo createInfo{
                .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, .pNext = nullptr, .flags = {}, .codeSize = (std::size_t)fileSize, .pCode = fileBuffer.getData()};
            if (!lib.createShaderModule(device, &createInfo, nullptr, &module))
                return false;

            return true;
        }
        void cleanup(VkDevice device)
        {
            if (module)
                lib.destroyShaderModule(device, module, nullptr);
        }

        [[nodiscard]] auto getModule() const { return module; }

    private:
        VkShaderModule module{};
    };

    class SingleUseStagingBuffer
    {
    public:
        SingleUseStagingBuffer() = default;

        [[nodiscard]] bool init(VkDevice device, PhysicalDeviceInfo const& physicalDeviceInfo, std::size_t size)
        {
            VkBufferCreateInfo bufferCreateInfo{.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                                                .pNext = nullptr,
                                                .flags = {},
                                                .size = size,
                                                .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                                                .queueFamilyIndexCount = 1,
                                                .pQueueFamilyIndices = &physicalDeviceInfo.graphicsIndex};
            if (!lib.createBuffer(device, &bufferCreateInfo, nullptr, &buffer))
                return false;

            VkMemoryRequirements memoryRequirements{};
            lib.getBufferMemoryRequirements(device, buffer, &memoryRequirements);

            VkMemoryPropertyFlags memoryProperties{VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

            auto selectedMemoryType = physicalDeviceInfo.getMemoryTypeIndex(memoryRequirements, memoryProperties);
            if (selectedMemoryType == -1)
                return false;

            VkMemoryAllocateInfo memoryAllocateInfo{.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                                                    .pNext = nullptr,
                                                    .allocationSize = memoryRequirements.size,
                                                    .memoryTypeIndex = (std::uint32_t)selectedMemoryType};
            if (!lib.allocateMemory(device, &memoryAllocateInfo, nullptr, &memory))
                return false;

            if (!lib.bindBufferMemory(device, buffer, memory, 0))
                return false;

            if (!lib.mapMemory(device, memory, 0, bufferCreateInfo.size, {}, &data))
                return false;

            return true;
        }

        void cleanup(VkDevice device)
        {
            if (data)
                lib.unmapMemory(device, memory);

            if (memory)
                lib.freeMemory(device, memory, nullptr);

            if (buffer)
                lib.destroyBuffer(device, buffer, nullptr);
        }

        [[nodiscard]] auto getBuffer() const { return buffer; }
        [[nodiscard]] auto getMemory() const { return memory; }
        [[nodiscard]] auto getData() const { return data; }

    private:
        VkBuffer buffer{};
        VkDeviceMemory memory{};
        void* data{};
    };

    // Class for creating and submitting one time command buffers
    class SingleUseCommandBuffer
    {
    public:
        SingleUseCommandBuffer() = default;

        // Create command buffer resources
        [[nodiscard]] bool init(VulkanLibrary const& lib, VkDevice device, VkCommandPool commandPool)
        {
            VkCommandBufferAllocateInfo allocateInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                                     .pNext = nullptr,
                                                     .commandPool = commandPool,
                                                     .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                                     .commandBufferCount = 1};
            if (!lib.allocateCommandBuffers(device, &allocateInfo, &commandBuffer))
                return false;

            VkCommandBufferBeginInfo beginInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                               .pNext = nullptr,
                                               .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                                               .pInheritanceInfo = nullptr};
            if (!lib.beginCommandBuffer(commandBuffer, &beginInfo))
                return false;

            return true;
        }

        void cleanup(VulkanLibrary const& lib, VkDevice device, VkCommandPool commandPool)
        {
            if (commandBuffer)
            {
                lib.freeCommandBuffers(device, commandPool, 1, &commandBuffer);
            }
        }

        // Submit command buffer
        [[nodiscard]] bool submit(VulkanLibrary const& lib, VkQueue submitQueue)
        {
            if (!lib.endCommandBuffer(commandBuffer))
                return false;

            VkSubmitInfo submitInfo{.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                    .pNext = nullptr,
                                    .waitSemaphoreCount = 0,
                                    .pWaitSemaphores = nullptr,
                                    .pWaitDstStageMask = nullptr,
                                    .commandBufferCount = 1,
                                    .pCommandBuffers = &commandBuffer,
                                    .signalSemaphoreCount = 0,
                                    .pSignalSemaphores = nullptr};
            if (!lib.queueSubmit(submitQueue, 1, &submitInfo, nullptr))
                return false;

            if (!lib.queueWaitIdle(submitQueue))
                return false;

            return true;
        }

        [[nodiscard]] auto getCommandBuffer() const { return commandBuffer; }

    private:
        VkCommandBuffer commandBuffer;
    };

    // Struct for shader constants
    struct PushConstantsBlock
    {
        VkDeviceAddress quadReference{};
    };

    class SynchronizationResources
    {
    public:
        SynchronizationResources() = default;

        [[nodiscard]] bool init(VulkanLibrary const& lib, VkDevice device, uint32_t imageCount)
        {
            // Create synchronization objects
            VkSemaphoreCreateInfo semaphoreCreateInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, .pNext = nullptr, .flags = {}};
            VkFenceCreateInfo fenceCreateInfo{.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .pNext = nullptr, .flags = VK_FENCE_CREATE_SIGNALED_BIT};
            for (std::uint32_t i{}; i < maxFramesInFlight; i++)
            {
                if (!lib.createSemaphore(device, &semaphoreCreateInfo, nullptr, &imageAcquiredSemaphores[i]) ||
                    !lib.createFence(device, &fenceCreateInfo, nullptr, &submitFinishedFences[i]))
                    return false;
            }

            submitFinishedSemaphores.resize(imageCount);
            for (std::uint32_t i{}; i < imageCount; i++)
            {
                if (!lib.createSemaphore(device, &semaphoreCreateInfo, nullptr, &submitFinishedSemaphores[i]))
                    return false;
            }
            logger.logInfo("Created synchronization objects");

            return true;
        }

        void cleanup(VulkanLibrary const& lib, VkDevice device)
        {
            for (std::uint32_t i{}; i < maxFramesInFlight; i++)
            {
                lib.destroySemaphore(device, imageAcquiredSemaphores[i], nullptr);
                lib.destroyFence(device, submitFinishedFences[i], nullptr);
            }

            for (std::uint32_t i{}; i < submitFinishedSemaphores.getSize(); i++)
                lib.destroySemaphore(device, submitFinishedSemaphores[i], nullptr);
        }

        [[nodiscard]] auto const& getImageSemaphore(std::uint64_t frameIndex) const { return imageAcquiredSemaphores[frameIndex]; }
        [[nodiscard]] auto const& getSubmitSemaphore(std::uint64_t imageIndex) const { return submitFinishedSemaphores[imageIndex]; }
        [[nodiscard]] auto const& getSubmitFence(std::uint64_t frameIndex) const { return submitFinishedFences[frameIndex]; }

    private:
        Array<VkSemaphore, maxFramesInFlight> imageAcquiredSemaphores;
        FixedVector<VkSemaphore, 8> submitFinishedSemaphores;
        Array<VkFence, maxFramesInFlight> submitFinishedFences;
    };

public:
    RenderEngine() = default;

    // Initialize Vulkan resources
    [[nodiscard]] bool init(std::string_view appName, std::uint32_t majorVersion, std::uint32_t minorVersion, std::uint32_t patchVersion)
    {
        if (!lib.init())
            return false;

        // Get available instance extensions
        Array<VkExtensionProperties, 256> availableInstanceExtensionProperties;
        std::uint32_t instanceExtensionCount{256};
        if (!lib.enumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, availableInstanceExtensionProperties.getData()))
            return false;

        // Print available instance extensions
        logger.logInfo("{} instance extensions available:", instanceExtensionCount);
        for (std::uint32_t i{}; i < instanceExtensionCount; i++)
            logger.logInfo("\t{}", availableInstanceExtensionProperties[i].extensionName);

        // Get available instance layers
        Array<VkLayerProperties, 256> availableInstanceLayerProperties;
        std::uint32_t instanceLayerCount{256};
        if (!lib.enumerateInstanceLayerProperties(&instanceLayerCount, availableInstanceLayerProperties.getData()))
            return false;

        // Print available instance layers
        logger.logInfo("{} validation layers available:", instanceLayerCount);
        for (std::uint32_t i{}; i < instanceLayerCount; i++)
            logger.logInfo("\t{}", availableInstanceLayerProperties[i].layerName);

        // Define application info
        VkApplicationInfo applicationInfo{.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                          .pNext = nullptr,
                                          .pApplicationName = appName.data(),
                                          .applicationVersion = VK_MAKE_VERSION(majorVersion, minorVersion, patchVersion),
                                          .pEngineName = "Quad Engine",
                                          .engineVersion = VK_MAKE_VERSION(0, 1, 0),
                                          .apiVersion = VK_MAKE_API_VERSION(0, 1, 4, 0)};

        // Define required instance layers
        FixedVector<char const*, 64> requiredLayers;
        if constexpr (isDebugBuild)
            requiredLayers.emplaceBack("VK_LAYER_KHRONOS_validation");
        logger.logInfo("{} Vulkan validation layers required:", requiredLayers.getSize());
        for (auto layer : requiredLayers)
            logger.logInfo("\t{}", layer);

        // Check support for required instance layers
        for (auto layer : requiredLayers)
        {
            bool isSupported{};
            for (std::uint32_t i{}; i < instanceLayerCount; i++)
            {
                if (std::strcmp(layer, availableInstanceLayerProperties[i].layerName) != 0)
                    continue;

                isSupported = true;
                break;
            }

            if (!isSupported)
            {
                logger.logError("Instance layer {} is not supported", layer);
                return false;
            }
        }

        // Define required instance extensions
        FixedVector<char const*, 64> requiredInstanceExtensions;
        for (auto extension : renderWindow.getRequiredExtensions())
            requiredInstanceExtensions.emplaceBack(extension);
        if constexpr (isDebugBuild)
            requiredInstanceExtensions.emplaceBack(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        // Print required instance extensions
        logger.logInfo("{} Vulkan instance extensions required:", requiredInstanceExtensions.getSize());
        for (auto extension : requiredInstanceExtensions)
            logger.logInfo("\t{}", extension);

        // Check support for required instance extensions
        for (auto extension : requiredInstanceExtensions)
        {
            bool isSupported{};
            for (std::uint32_t i{}; i < instanceExtensionCount; i++)
            {
                if (std::strcmp(extension, availableInstanceExtensionProperties[i].extensionName) != 0)
                    continue;

                isSupported = true;
                break;
            }

            if (!isSupported)
            {
                logger.logError("Instance extension {} is not supported", extension);
                return false;
            }
        }

        // Create Vulkan instance
        VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .pNext = nullptr,
            .flags = {},
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = debugCallback,
            .pUserData = nullptr};
        VkInstanceCreateInfo instanceCreateInfo{.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                                .pNext = nullptr,
                                                .flags = {},
                                                .pApplicationInfo = &applicationInfo,
                                                .enabledLayerCount = (uint32_t)requiredLayers.getSize(),
                                                .ppEnabledLayerNames = requiredLayers.getData(),
                                                .enabledExtensionCount = (uint32_t)requiredInstanceExtensions.getSize(),
                                                .ppEnabledExtensionNames = requiredInstanceExtensions.getData()};
        if constexpr (isDebugBuild)
            instanceCreateInfo.pNext = &messengerCreateInfo;
        if (!lib.createInstance(&instanceCreateInfo, nullptr, &instance))
            return false;

        if (!lib.init(instance))
            return false;

        // Create debug messenger
        if constexpr (isDebugBuild)
        {
            if (!lib.createDebugUtilsMessenger(instance, &messengerCreateInfo, nullptr, &debugMessenger))
                return false;
        }

        // Create window surface
        if (!renderWindow.createSurface(instance, nullptr, &surface))
            return false;

        // Get available physical devices
        Array<VkPhysicalDevice, 64> availablePhysicalDevices;
        std::uint32_t availablePhysicalDeviceCount{64};
        if (!lib.enumeratePhysicalDevices(instance, &availablePhysicalDeviceCount, availablePhysicalDevices.getData()))
            return false;
        if (availablePhysicalDeviceCount == 0)
        {
            logger.logError("No physical device with Vulkan support found. Try updating drivers");
            return false;
        }

        // Print available physical devices
        logger.logInfo("{} physical devices available:", availablePhysicalDeviceCount);
        for (std::uint32_t i{}; i < availablePhysicalDeviceCount; i++)
        {
            VkPhysicalDeviceProperties properties;
            lib.getPhysicalDeviceProperties(availablePhysicalDevices[i], &properties);
            logger.logInfo("\t{}", properties.deviceName);
        }

        // Define required physical device extensions
        FixedVector<char const*, 64> requiredPhysicalDeviceExtensions;
        requiredPhysicalDeviceExtensions.emplaceBack(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        // Print required physical device extensions
        logger.logInfo("{} physical device extensions required:", requiredPhysicalDeviceExtensions.getSize());
        for (auto const& extension : requiredPhysicalDeviceExtensions)
            logger.logInfo("\t{}", extension);

        // Choose the best physical device
        std::uint32_t maxDeviceScore{};
        for (std::uint32_t i = 0; i < availablePhysicalDeviceCount; i++)
        {
            auto currentPhysicalDevice = availablePhysicalDevices[i];
            std::uint32_t currentScore{};
            PhysicalDeviceInfo currentInfo;
            auto& [name, formatCount, formats, presentModeCount, presentModes, surfaceCapabilities, graphicsIndex, presentationIndex, maxSamplerAnisotropy,
                   memoryProperties] = currentInfo;

            // Get physical device properties
            VkPhysicalDeviceProperties deviceProperties;
            lib.getPhysicalDeviceProperties(currentPhysicalDevice, &deviceProperties);
            name = deviceProperties.deviceName;
            maxSamplerAnisotropy = deviceProperties.limits.maxSamplerAnisotropy;
            logger.logInfo("Checking {} suitability:", name);

            // Get queue family properties
            Array<VkQueueFamilyProperties, 16> queueFamilyProperties;
            std::uint32_t queueFamilyPropertiesCount{16};
            lib.getPhysicalDeviceQueueFamilyProperties(currentPhysicalDevice, &queueFamilyPropertiesCount, queueFamilyProperties.getData());
            logger.logInfo("\t{} queue families available", queueFamilyPropertiesCount);

            // Check queue graphics, surface and presentation support
            bool hasGraphicsQueueFamily{};
            bool hasPresentationQueueFamily{};
            for (std::uint32_t j{}; j < queueFamilyPropertiesCount; j++)
            {
                if (queueFamilyProperties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    hasGraphicsQueueFamily = true;
                    graphicsIndex = j;
                    logger.logInfo("\tQueue family with index {} supports graphics", j);
                }

                VkBool32 surfaceSupport;
                if (!lib.getPhysicalDeviceSurfaceSupport(currentPhysicalDevice, j, surface, &surfaceSupport))
                    return false;

                if (surfaceSupport)
                {
                    hasPresentationQueueFamily = true;
                    presentationIndex = j;
                    logger.logInfo("\tQueue family with index {} supports presentation", j);
                }

                if (hasGraphicsQueueFamily && hasPresentationQueueFamily)
                    break;
            }
            if (!hasGraphicsQueueFamily)
            {
                logger.logInfo("\tNo queue family with graphics support found");
                continue;
            }
            if (!hasPresentationQueueFamily)
            {
                logger.logInfo("\tNo queue family with presentation support found");
                continue;
            }

            Array<VkExtensionProperties, 1024> deviceExtensionProperties;
            std::uint32_t deviceExtensionPropertiesCount{1024};
            if (!lib.enumerateDeviceExtensionProperties(currentPhysicalDevice, nullptr, &deviceExtensionPropertiesCount, deviceExtensionProperties.getData()))
                return false;
            logger.logInfo("\t{} physical device extensions available:", deviceExtensionPropertiesCount);
            for (std::uint32_t j{}; j < deviceExtensionPropertiesCount; j++)
                logger.logInfo("\t\t{}", deviceExtensionProperties[j].extensionName);

            bool hasUnsupportedExtension{};
            for (auto extension : requiredPhysicalDeviceExtensions)
            {
                bool isSupported{};
                for (auto const& property : deviceExtensionProperties)
                {
                    if (std::strcmp(property.extensionName, extension) != 0)
                        continue;

                    isSupported = true;
                    break;
                }

                if (!isSupported)
                {
                    logger.logInfo("\tRequired physical device extension {} not supported", extension);
                    hasUnsupportedExtension = true;
                    break;
                }
            }
            if (hasUnsupportedExtension)
                continue;

            formatCount = formats.getSize();
            if (!lib.getPhysicalDeviceSurfaceFormats(currentPhysicalDevice, surface, &formatCount, formats.getData()))
                return false;
            logger.logInfo("\t{} physical device surface formats available:", formatCount);
            for (std::uint32_t j{}; j < formatCount; j++)
                logger.logInfo("\t\t{} {}", enumToStr(formats[j].format), enumToStr(formats[j].colorSpace));

            presentModeCount = presentModes.getSize();
            if (!lib.getPhysicalDeviceSurfacePresentModes(currentPhysicalDevice, surface, &presentModeCount, presentModes.getData()))
                return false;
            logger.logInfo("\t{} physical device present modes available:", presentModeCount);
            for (std::uint32_t j{}; j < presentModeCount; j++)
                logger.logInfo("\t\t{}", enumToStr(presentModes[j]));

            if (!lib.getPhysicalDeviceSurfaceCapabilities(currentPhysicalDevice, surface, &surfaceCapabilities))
                return false;
            logger.logInfo("\tPhysical device minimum surface image count is {}", surfaceCapabilities.minImageCount);
            if (surfaceCapabilities.maxImageCount == 0)
                logger.logInfo("\tPhysical device surface image count has no maximum");
            else
                logger.logInfo("\tPhysical device maximum surface image count is {}", surfaceCapabilities.maxImageCount);

            lib.getPhysicalDeviceMemoryProperties(currentPhysicalDevice, &memoryProperties);

            if (formatCount == 0 || presentModeCount == 0)
            {
                logger.logInfo("\tPhysical device doesn't support swapchain");
                continue;
            }

            VkPhysicalDeviceVulkan12Features features12;
            features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
            features12.pNext = nullptr;
            VkPhysicalDeviceVulkan11Features features11;
            features11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
            features11.pNext = &features12;
            VkPhysicalDeviceFeatures2 features{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = &features11, .features = {}};
            lib.getPhysicalDeviceFeatures2(currentPhysicalDevice, &features);
            if (!features.features.shaderInt64)
            {
                logger.logInfo("\tPhysical device doesn't support 64 bit integers");
                continue;
            }
            if (!features.features.samplerAnisotropy)
            {
                logger.logInfo("\tPhysical device doesn't support anisotropic filtering");
                continue;
            }
            if (!features12.scalarBlockLayout)
            {
                logger.logInfo("\tPhysical device doesn't support scalar block layout");
                continue;
            }
            if (!features12.bufferDeviceAddress)
            {
                logger.logInfo("\tPhysical device doesn't support buffer device address");
                continue;
            }

            if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                currentScore++;
            logger.logInfo("\tPhysical device is a {}", enumToStr(deviceProperties.deviceType));

            if (currentScore > maxDeviceScore)
            {
                maxDeviceScore = currentScore;
                physicalDevice = currentPhysicalDevice;
                physicalDeviceInfo = currentInfo;
            }
        }
        if (!physicalDevice)
        {
            logger.logError("No suitable physical devices found. Try updating drivers");
            return false;
        }
        logger.logInfo("Picked {} as a suitable physical device", physicalDeviceInfo.name);

        // Define device queues
        float queuePriority{1.0f};
        FixedVector<VkDeviceQueueCreateInfo, 16> queueCreateInfos;
        queueCreateInfos.emplaceBack(VkDeviceQueueCreateInfo{.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                                             .pNext = nullptr,
                                                             .flags = {},
                                                             .queueFamilyIndex = physicalDeviceInfo.graphicsIndex,
                                                             .queueCount = 1,
                                                             .pQueuePriorities = &queuePriority});
        if (physicalDeviceInfo.graphicsIndex != physicalDeviceInfo.presentationIndex)
        {
            queueCreateInfos.emplaceBack(VkDeviceQueueCreateInfo{.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                                                 .pNext = nullptr,
                                                                 .flags = {},
                                                                 .queueFamilyIndex = physicalDeviceInfo.presentationIndex,
                                                                 .queueCount = 1,
                                                                 .pQueuePriorities = &queuePriority});
        }

        // Create logical device
        VkPhysicalDeviceVulkan12Features features12{};
        features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        features12.bufferDeviceAddress = VK_TRUE;
        features12.scalarBlockLayout = VK_TRUE;
        VkPhysicalDeviceVulkan11Features features11{};
        features11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
        features11.pNext = &features12;
        VkPhysicalDeviceFeatures2 requiredPhysicalDeviceFeatures{.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, .pNext = &features11, .features = {}};
        requiredPhysicalDeviceFeatures.features.shaderInt64 = VK_TRUE;
        requiredPhysicalDeviceFeatures.features.samplerAnisotropy = VK_TRUE;
        VkDeviceCreateInfo deviceCreateInfo{.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                                            .pNext = &requiredPhysicalDeviceFeatures,
                                            .flags = {},
                                            .queueCreateInfoCount = (std::uint32_t)queueCreateInfos.getSize(),
                                            .pQueueCreateInfos = queueCreateInfos.getData(),
                                            .enabledLayerCount = 0,
                                            .ppEnabledLayerNames = nullptr,
                                            .enabledExtensionCount = (std::uint32_t)requiredPhysicalDeviceExtensions.getSize(),
                                            .ppEnabledExtensionNames = requiredPhysicalDeviceExtensions.getData(),
                                            .pEnabledFeatures = nullptr};
        if (!lib.createDevice(physicalDevice, &deviceCreateInfo, nullptr, &device))
            return false;

        if (!lib.init(device))
            return false;

        // Get queues
        lib.getDeviceQueue(device, physicalDeviceInfo.graphicsIndex, 0, &graphicsQueue);
        lib.getDeviceQueue(device, physicalDeviceInfo.presentationIndex, 0, &presentationQueue);

        if (!swapchainResources.init(lib, physicalDeviceInfo, device, surface, {}))
            return false;

        // Create command pool
        VkCommandPoolCreateInfo poolCreateInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                                               .pNext = nullptr,
                                               .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                                               .queueFamilyIndex = physicalDeviceInfo.graphicsIndex};
        if (!lib.createCommandPool(device, &poolCreateInfo, nullptr, &commandPool))
            return false;

        // Create texture resources
        {
            auto tileImage = ImageLoader("textures/tiles.png"sv);
            VkDeviceSize imageSize{(std::size_t)tileImage.width * tileImage.height * tileImage.channels};

            SingleUseStagingBuffer stagingBufferResources;
            if (!stagingBufferResources.init(device, physicalDeviceInfo, imageSize))
                return false;
            std::memcpy(stagingBufferResources.getData(), tileImage.data, imageSize);

            VkImageCreateInfo imageCreateInfo{.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                                              .pNext = nullptr,
                                              .flags = {},
                                              .imageType = VK_IMAGE_TYPE_2D,
                                              .format = VK_FORMAT_R8_UNORM,
                                              .extent = VkExtent3D{(std::uint32_t)tileImage.width, (std::uint32_t)tileImage.height, 1u},
                                              .mipLevels = 1,
                                              .arrayLayers = 1,
                                              .samples = VK_SAMPLE_COUNT_1_BIT,
                                              .tiling = VK_IMAGE_TILING_OPTIMAL,
                                              .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                              .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                                              .queueFamilyIndexCount = 1,
                                              .pQueueFamilyIndices = &physicalDeviceInfo.graphicsIndex,
                                              .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED};
            if (!lib.createImage(device, &imageCreateInfo, nullptr, &textureImage))
                return false;

            VkMemoryRequirements memoryRequirements{};
            lib.getImageMemoryRequirements(device, textureImage, &memoryRequirements);
            VkMemoryAllocateInfo imageMemoryAllocateInfo{.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                                                         .pNext = nullptr,
                                                         .allocationSize = memoryRequirements.size,
                                                         .memoryTypeIndex =
                                                             (std::uint32_t)physicalDeviceInfo.getMemoryTypeIndex(memoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)};
            if (!lib.allocateMemory(device, &imageMemoryAllocateInfo, nullptr, &textureImageMemory))
                return false;

            if (!lib.bindImageMemory(device, textureImage, textureImageMemory, 0))
                return false;

            {
                SingleUseCommandBuffer transitionCommandBuffer;
                if (!transitionCommandBuffer.init(lib, device, commandPool))
                    return false;

                VkImageSubresourceRange range(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1);
                VkImageMemoryBarrier memoryBarrier{.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                   .pNext = nullptr,
                                                   .srcAccessMask = VK_ACCESS_NONE,
                                                   .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                                                   .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                                   .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                   .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                   .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                   .image = textureImage,
                                                   .subresourceRange = range};
                lib.commandPipelineBarrier(transitionCommandBuffer.getCommandBuffer(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, {}, 0,
                                           nullptr, 0, nullptr, 1, &memoryBarrier);

                if (!transitionCommandBuffer.submit(lib, graphicsQueue))
                    return false;
            }

            {
                SingleUseCommandBuffer copyCommandBuffer;
                if (!copyCommandBuffer.init(lib, device, commandPool))
                    return false;

                VkImageSubresourceLayers imageSubresourceLayers(VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1);
                VkBufferImageCopy imageCopy{.bufferOffset = 0,
                                            .bufferRowLength = 0,
                                            .bufferImageHeight = 0,
                                            .imageSubresource = imageSubresourceLayers,
                                            .imageOffset = {},
                                            .imageExtent = {(std::uint32_t)tileImage.width, (std::uint32_t)tileImage.height, 1}};
                lib.commandCopyBufferToImage(copyCommandBuffer.getCommandBuffer(), stagingBufferResources.getBuffer(), textureImage,
                                             VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopy);

                if (!copyCommandBuffer.submit(lib, graphicsQueue))
                    return false;
            }

            {
                SingleUseCommandBuffer transitionCommandBuffer;
                if (!transitionCommandBuffer.init(lib, device, commandPool))
                    return false;

                VkImageSubresourceRange range(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1);
                VkImageMemoryBarrier memoryBarrier{.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                   .pNext = nullptr,
                                                   .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                                                   .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
                                                   .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                   .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                   .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                   .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                   .image = textureImage,
                                                   .subresourceRange = range};
                lib.commandPipelineBarrier(transitionCommandBuffer.getCommandBuffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                           {}, 0, nullptr, 0, nullptr, 1, &memoryBarrier);

                if (!transitionCommandBuffer.submit(lib, graphicsQueue))
                    return false;
            }

            VkImageSubresourceRange subresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1);
            VkImageViewCreateInfo viewCreateInfo{.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                                                 .pNext = nullptr,
                                                 .flags = {},
                                                 .image = textureImage,
                                                 .viewType = VK_IMAGE_VIEW_TYPE_2D,
                                                 .format = VK_FORMAT_R8_UNORM,
                                                 .components = {},
                                                 .subresourceRange = subresourceRange};
            if (!lib.createImageView(device, &viewCreateInfo, nullptr, &textureImageView))
                return false;

            VkSamplerCreateInfo samplerCreateInfo{.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                                                  .pNext = nullptr,
                                                  .flags = {},
                                                  .magFilter = VK_FILTER_LINEAR,
                                                  .minFilter = VK_FILTER_LINEAR,
                                                  .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
                                                  .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                                                  .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                                                  .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
                                                  .mipLodBias = 0.0f,
                                                  .anisotropyEnable = VK_TRUE,
                                                  .maxAnisotropy = physicalDeviceInfo.maxSamplerAnisotropy,
                                                  .compareEnable = VK_FALSE,
                                                  .compareOp = VK_COMPARE_OP_ALWAYS,
                                                  .minLod = 0.0f,
                                                  .maxLod = 0.0f,
                                                  .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
                                                  .unnormalizedCoordinates = VK_FALSE};
            if (!lib.createSampler(device, &samplerCreateInfo, nullptr, &textureSampler))
                return false;

            stagingBufferResources.cleanup(device);
        }

        VkDescriptorSetLayoutBinding layoutBinding{0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr};
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, .pNext = nullptr, .flags = {}, .bindingCount = 1, .pBindings = &layoutBinding};
        if (!lib.createDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout))
            return false;

        VkDescriptorPoolSize descriptorPoolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, maxFramesInFlight};
        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                                                            .pNext = nullptr,
                                                            .flags = {},
                                                            .maxSets = maxFramesInFlight,
                                                            .poolSizeCount = 1,
                                                            .pPoolSizes = &descriptorPoolSize};
        if (!lib.createDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool))
            return false;

        Array<VkDescriptorSetLayout, maxFramesInFlight> setLayouts{descriptorSetLayout, descriptorSetLayout};
        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                                                              .pNext = nullptr,
                                                              .descriptorPool = descriptorPool,
                                                              .descriptorSetCount = maxFramesInFlight,
                                                              .pSetLayouts = setLayouts.getData()};
        if (!lib.allocateDescriptorSets(device, &descriptorSetAllocateInfo, descriptorSets.getData()))
            return false;

        for (size_t i = 0; i < maxFramesInFlight; i++)
        {
            VkDescriptorImageInfo imageInfo(textureSampler, textureImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            VkWriteDescriptorSet writeDescriptorSet{.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                                    .pNext = nullptr,
                                                    .dstSet = descriptorSets[i],
                                                    .dstBinding = 0,
                                                    .dstArrayElement = 0,
                                                    .descriptorCount = 1,
                                                    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                    .pImageInfo = &imageInfo,
                                                    .pBufferInfo = nullptr,
                                                    .pTexelBufferView = nullptr};
            lib.updateDescriptorSets(device, 1, &writeDescriptorSet, 0, nullptr);
        }

        {
            // Create shader modules
            ShaderModule vertexShaderModule;
            if (!vertexShaderModule.init(device, "shaders/quadVert.spv"sv))
                return false;
            ShaderModule fragmentShaderModule;
            if (!fragmentShaderModule.init(device, "shaders/quadFrag.spv"sv))
                return false;

            // Define shader stages
            Array stageCreateInfos{VkPipelineShaderStageCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                                                   .pNext = nullptr,
                                                                   .flags = {},
                                                                   .stage = VK_SHADER_STAGE_VERTEX_BIT,
                                                                   .module = vertexShaderModule.getModule(),
                                                                   .pName = "main",
                                                                   .pSpecializationInfo = nullptr},
                                   VkPipelineShaderStageCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                                                   .pNext = nullptr,
                                                                   .flags = {},
                                                                   .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                   .module = fragmentShaderModule.getModule(),
                                                                   .pName = "main",
                                                                   .pSpecializationInfo = nullptr}};

            // Define dynamic states
            Array dynamicStates{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
            VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
                                                                    .pNext = nullptr,
                                                                    .flags = {},
                                                                    .dynamicStateCount = dynamicStates.getSize(),
                                                                    .pDynamicStates = dynamicStates.getData()};

            // Define vertex input
            VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                                                                            .pNext = nullptr,
                                                                            .flags = {},
                                                                            .vertexBindingDescriptionCount = 0,
                                                                            .pVertexBindingDescriptions = nullptr,
                                                                            .vertexAttributeDescriptionCount = 0,
                                                                            .pVertexAttributeDescriptions = nullptr};

            // Define input assembly
            VkPipelineInputAssemblyStateCreateInfo assemblyStateCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                                                                           .pNext = nullptr,
                                                                           .flags = {},
                                                                           .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
                                                                           .primitiveRestartEnable = VK_FALSE};

            // Define viewport
            VkViewport viewport{0.0f, 0.0f, (float)swapchainResources.imageExtent.width, (float)swapchainResources.imageExtent.height, 0.0f, 1.0f};
            VkRect2D scissor{{0, 0}, swapchainResources.imageExtent};
            VkPipelineViewportStateCreateInfo viewportStateCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                                                                      .pNext = nullptr,
                                                                      .flags = {},
                                                                      .viewportCount = 1,
                                                                      .pViewports = &viewport,
                                                                      .scissorCount = 1,
                                                                      .pScissors = &scissor};

            // Define rasterization
            VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                                                                                .pNext = nullptr,
                                                                                .flags = {},
                                                                                .depthClampEnable = VK_FALSE,
                                                                                .rasterizerDiscardEnable = VK_FALSE,
                                                                                .polygonMode = VK_POLYGON_MODE_FILL,
                                                                                .cullMode = VK_CULL_MODE_BACK_BIT,
                                                                                .frontFace = VK_FRONT_FACE_CLOCKWISE,
                                                                                .depthBiasEnable = VK_FALSE,
                                                                                .depthBiasConstantFactor = 0.0f,
                                                                                .depthBiasClamp = 0.0f,
                                                                                .depthBiasSlopeFactor = 0.0f,
                                                                                .lineWidth = 1.0f};

            // Define multisampling
            VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                                                                            .pNext = nullptr,
                                                                            .flags = {},
                                                                            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
                                                                            .sampleShadingEnable = VK_FALSE,
                                                                            .minSampleShading = 0.0,
                                                                            .pSampleMask = nullptr,
                                                                            .alphaToCoverageEnable = VK_FALSE,
                                                                            .alphaToOneEnable = VK_FALSE};

            // Define depth and stencil
            VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
                                                                              .pNext = nullptr,
                                                                              .flags = {},
                                                                              .depthTestEnable = VK_FALSE,
                                                                              .depthWriteEnable = VK_FALSE,
                                                                              .depthCompareOp = VK_COMPARE_OP_NEVER,
                                                                              .depthBoundsTestEnable = VK_FALSE,
                                                                              .stencilTestEnable = VK_FALSE,
                                                                              .front = {},
                                                                              .back = {},
                                                                              .minDepthBounds = 0.0f,
                                                                              .maxDepthBounds = 1.0f};

            // Define color blending
            VkPipelineColorBlendAttachmentState colorBlendAttachmentState{.blendEnable = VK_TRUE,
                                                                          .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
                                                                          .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                                                                          .colorBlendOp = VK_BLEND_OP_ADD,
                                                                          .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                                                                          .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                                                                          .alphaBlendOp = VK_BLEND_OP_ADD,
                                                                          .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                                                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT};
            VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                                                                          .pNext = nullptr,
                                                                          .flags = {},
                                                                          .logicOpEnable = VK_FALSE,
                                                                          .logicOp = VK_LOGIC_OP_NO_OP,
                                                                          .attachmentCount = 1,
                                                                          .pAttachments = &colorBlendAttachmentState,
                                                                          .blendConstants = {1.0f, 1.0f, 1.0f, 1.0f}};

            // Create pipeline layout
            VkPushConstantRange pushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(QuadData));
            VkPipelineLayoutCreateInfo layoutCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                                                        .pNext = nullptr,
                                                        .flags = {},
                                                        .setLayoutCount = 1,
                                                        .pSetLayouts = &descriptorSetLayout,
                                                        .pushConstantRangeCount = 1,
                                                        .pPushConstantRanges = &pushConstantRange};
            if (!lib.createPipelineLayout(device, &layoutCreateInfo, nullptr, &pipelineLayout))
                return false;

            // Create graphics pipeline
            VkGraphicsPipelineCreateInfo pipelineCreateInfo{.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                                                            .pNext = nullptr,
                                                            .flags = {},
                                                            .stageCount = stageCreateInfos.getSize(),
                                                            .pStages = stageCreateInfos.getData(),
                                                            .pVertexInputState = &vertexInputStateCreateInfo,
                                                            .pInputAssemblyState = &assemblyStateCreateInfo,
                                                            .pTessellationState = nullptr,
                                                            .pViewportState = &viewportStateCreateInfo,
                                                            .pRasterizationState = &rasterizationStateCreateInfo,
                                                            .pMultisampleState = &multisampleStateCreateInfo,
                                                            .pDepthStencilState = &depthStencilStateCreateInfo,
                                                            .pColorBlendState = &colorBlendStateCreateInfo,
                                                            .pDynamicState = &dynamicStateCreateInfo,
                                                            .layout = pipelineLayout,
                                                            .renderPass = swapchainResources.renderPass,
                                                            .subpass = 0,
                                                            .basePipelineHandle = nullptr,
                                                            .basePipelineIndex = 0};
            if (!lib.createGraphicsPipelines(device, {}, 1, &pipelineCreateInfo, nullptr, &graphicsPipeline))
                return false;

            vertexShaderModule.cleanup(device);
            fragmentShaderModule.cleanup(device);
        }

        for (std::uint64_t i{}; i < quadDataBuffers.getSize(); i++)
        {
            auto& bufferResources = quadDataBuffers[i];

            VkBufferCreateInfo bufferCreateInfo{.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                                                .pNext = nullptr,
                                                .flags = {},
                                                .size = sizeof(QuadData) * QuadPool::getCapacity(),
                                                .usage = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
                                                .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                                                .queueFamilyIndexCount = 1,
                                                .pQueueFamilyIndices = &physicalDeviceInfo.graphicsIndex};
            if (!lib.createBuffer(device, &bufferCreateInfo, nullptr, &bufferResources.buffer))
                return false;

            VkMemoryRequirements bufferMemoryRequirements{};
            lib.getBufferMemoryRequirements(device, bufferResources.buffer, &bufferMemoryRequirements);

            VkMemoryPropertyFlags memoryProperties{VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

            auto selectedMemoryType = physicalDeviceInfo.getMemoryTypeIndex(bufferMemoryRequirements, memoryProperties);
            if (selectedMemoryType == -1)
                return false;

            VkMemoryAllocateFlagsInfo memoryAllocateFlagsInfo{
                .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO, .pNext = nullptr, .flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT, .deviceMask = 0};
            VkMemoryAllocateInfo memoryAllocateInfo{.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                                                    .pNext = &memoryAllocateFlagsInfo,
                                                    .allocationSize = bufferMemoryRequirements.size,
                                                    .memoryTypeIndex = (std::uint32_t)selectedMemoryType};
            if (!lib.allocateMemory(device, &memoryAllocateInfo, nullptr, &bufferResources.bufferMemory))
                return false;

            if (!lib.bindBufferMemory(device, bufferResources.buffer, bufferResources.bufferMemory, 0))
                return false;

            VkBufferDeviceAddressInfo deviceAddressInfo{
                .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO, .pNext = nullptr, .buffer = bufferResources.buffer};
            bufferResources.bufferAddress = lib.getBufferDeviceAddress(device, &deviceAddressInfo);
            if (!bufferResources.bufferAddress)
            {
                logger.logError("Failed to get buffer address");
                return false;
            }

            if (!lib.mapMemory(device, bufferResources.bufferMemory, 0, bufferCreateInfo.size, {}, &bufferResources.data))
                return false;
        }
        logger.logInfo("Created quad data buffers");

        // Allocate command buffers
        VkCommandBufferAllocateInfo bufferAllocateInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                                       .pNext = nullptr,
                                                       .commandPool = commandPool,
                                                       .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                                       .commandBufferCount = maxFramesInFlight};
        if (!lib.allocateCommandBuffers(device, &bufferAllocateInfo, commandBuffers.getData()))
            return false;

        // Create synchronization objects
        if (!synchronizationResources.init(lib, device, swapchainResources.imageCount))
            return false;
        logger.logInfo("Created synchronization objects");

        return true;
    }
    void cleanup()
    {
        if (device)
        {
            auto _ = lib.deviceWaitIdle(device);

            synchronizationResources.cleanup(lib, device);
            lib.freeCommandBuffers(device, commandPool, commandBuffers.getSize(), commandBuffers.getData());

            for (auto buffer : quadDataBuffers)
            {
                lib.freeMemory(device, buffer.bufferMemory, nullptr);
                lib.destroyBuffer(device, buffer.buffer, nullptr);
            }

            lib.destroyPipeline(device, graphicsPipeline, nullptr);
            lib.destroyPipelineLayout(device, pipelineLayout, nullptr);

            lib.destroyDescriptorPool(device, descriptorPool, nullptr);
            lib.destroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

            lib.destroySampler(device, textureSampler, nullptr);
            lib.destroyImageView(device, textureImageView, nullptr);
            lib.freeMemory(device, textureImageMemory, nullptr);
            lib.destroyImage(device, textureImage, nullptr);

            lib.destroyCommandPool(device, commandPool, nullptr);

            if (swapchainResources.swapchain)
            {
                swapchainResources.cleanup(lib, device);
            }

            lib.destroyDevice(device, nullptr);
        }

        renderWindow.destroySurface(instance, surface, nullptr);
        renderWindow.cleanup();

        if (debugMessenger)
            lib.destroyDebugUtilsMessenger(instance, debugMessenger, nullptr);

        if (instance)
            lib.destroyInstance(instance, nullptr);

        lib.cleanup();
    }

    // Submit drawing commands for current frame
    [[nodiscard]] bool drawFrame()
    {
        auto timeout = std::numeric_limits<std::uint64_t>::max();
        if (!lib.waitForFences(device, 1, &synchronizationResources.getSubmitFence(currentFrameIndex), VK_TRUE, timeout))
            return false;
        if (!lib.resetFences(device, 1, &synchronizationResources.getSubmitFence(currentFrameIndex)))
            return false;

        std::uint32_t imageIndex{};
        auto result =
            lib.acquireNextImage(device, swapchainResources.swapchain, timeout, synchronizationResources.getImageSemaphore(currentFrameIndex), {}, &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
            return recreateSwapchain();

        if (checkError(result, "", "Failed to acquire next image"))
            return false;

        auto commandBuffer = commandBuffers[currentFrameIndex];

        if (!lib.resetCommandBuffer(commandBuffer, {}))
            return false;

        auto instanceCount = copyQuadPools();

        VkCommandBufferBeginInfo beginInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, .pNext = nullptr, .flags = {}, .pInheritanceInfo = nullptr};
        if (!lib.beginCommandBuffer(commandBuffer, &beginInfo))
            return false;

        VkRect2D renderArea({0, 0}, swapchainResources.imageExtent);
        VkClearValue clearValue(VkClearColorValue{0.0f, 0.0f, 0.0f, 1.0f});
        VkRenderPassBeginInfo renderPassBeginInfo{.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                                                  .pNext = nullptr,
                                                  .renderPass = swapchainResources.renderPass,
                                                  .framebuffer = swapchainResources.framebuffers[imageIndex],
                                                  .renderArea = renderArea,
                                                  .clearValueCount = 1,
                                                  .pClearValues = &clearValue};
        lib.commandBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        lib.commandBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        VkViewport viewport(0.0f, 0.0f, swapchainResources.imageExtent.width, swapchainResources.imageExtent.height, 0.0f, 1.0f);
        lib.commandSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{.offset = {0, 0}, .extent = swapchainResources.imageExtent};
        lib.commandSetScissor(commandBuffer, 0, 1, &scissor);

        PushConstantsBlock pushConstants{quadDataBuffers[currentFrameIndex].bufferAddress};
        lib.commandPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0u, sizeof(pushConstants), &pushConstants);

        lib.commandBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[currentFrameIndex], 0, nullptr);
        lib.commandDraw(commandBuffer, 4, instanceCount, 0, 0);

        lib.commandEndRenderPass(commandBuffer);

        if (!lib.endCommandBuffer(commandBuffer))
            return false;

        VkPipelineStageFlags waitStage{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkSubmitInfo submitInfo{.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                .pNext = nullptr,
                                .waitSemaphoreCount = 1,
                                .pWaitSemaphores = &synchronizationResources.getImageSemaphore(currentFrameIndex),
                                .pWaitDstStageMask = &waitStage,
                                .commandBufferCount = 1,
                                .pCommandBuffers = &commandBuffers[currentFrameIndex],
                                .signalSemaphoreCount = 1,
                                .pSignalSemaphores = &synchronizationResources.getSubmitSemaphore(imageIndex)};
        if (!lib.queueSubmit(graphicsQueue, 1, &submitInfo, synchronizationResources.getSubmitFence(currentFrameIndex)))
            return false;

        VkPresentInfoKHR presentInfo{.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                                     .pNext = nullptr,
                                     .waitSemaphoreCount = 1,
                                     .pWaitSemaphores = &synchronizationResources.getSubmitSemaphore(imageIndex),
                                     .swapchainCount = 1,
                                     .pSwapchains = &swapchainResources.swapchain,
                                     .pImageIndices = &imageIndex,
                                     .pResults = nullptr};
        result = lib.queuePresent(presentationQueue, &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
            return recreateSwapchain();

        if (checkError(result, "", "Failed to present image"))
            return false;

        currentFrameIndex = (currentFrameIndex + 1) % maxFramesInFlight;

        return true;
    }

private:
    template <std::size_t currentIndex = 0>
    std::size_t copyQuadPools(std::size_t instanceCount = 0)
    {
        if constexpr (currentIndex < (std::size_t)QuadLayer::COUNT)
        {
            auto poolData = quadPool.getData<(QuadLayer)currentIndex>();
            std::memcpy(static_cast<char*>(quadDataBuffers[currentFrameIndex].data) + instanceCount * sizeof(QuadData), poolData.data(),
                        poolData.size() * sizeof(QuadData));
            return copyQuadPools<currentIndex + 1>(instanceCount + poolData.size());
        }
        else
        {
            return instanceCount;
        }
    }

    // Create new swapchain and mark old one for deletion
    [[nodiscard]] bool recreateSwapchain()
    {
        if (!lib.getPhysicalDeviceSurfaceCapabilities(physicalDevice, surface, &physicalDeviceInfo.surfaceCapabilities))
            return false;

        if (physicalDeviceInfo.surfaceCapabilities.currentExtent.width == 0 || physicalDeviceInfo.surfaceCapabilities.currentExtent.height == 0)
            return true;

        if (!lib.getPhysicalDeviceSurfaceFormats(physicalDevice, surface, &physicalDeviceInfo.surfaceFormatCount, physicalDeviceInfo.surfaceFormats.getData()))
            return false;

        if (!lib.getPhysicalDeviceSurfacePresentModes(physicalDevice, surface, &physicalDeviceInfo.presentModeCount, physicalDeviceInfo.presentModes.getData()))
            return false;

        auto oldSwapchainResources = swapchainResources;
        if (!swapchainResources.init(lib, physicalDeviceInfo, device, surface, oldSwapchainResources.swapchain))
            return false;

        if (!lib.deviceWaitIdle(device))
            return false;

        synchronizationResources.cleanup(lib, device);
        oldSwapchainResources.cleanup(lib, device);
        lib.freeCommandBuffers(device, commandPool, maxFramesInFlight, commandBuffers.getData());

        // Allocate command buffers
        VkCommandBufferAllocateInfo bufferAllocateInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                                       .pNext = nullptr,
                                                       .commandPool = commandPool,
                                                       .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                                       .commandBufferCount = maxFramesInFlight};
        if (!lib.allocateCommandBuffers(device, &bufferAllocateInfo, commandBuffers.getData()))
            return false;

        return synchronizationResources.init(lib, device, swapchainResources.imageCount);
    }

    // Callback for debug utils messenger
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        VkDebugUtilsMessengerCallbackDataEXT const* pCallbackData, void* pUserData)
    {
        if (messageSeverity <= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
            logger.logInfo("{}", pCallbackData->pMessage);
        else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
            logger.logError("{}", pCallbackData->pMessage);

        return VK_FALSE;
    }

    VkInstance instance{};
    VkDebugUtilsMessengerEXT debugMessenger{};
    VkSurfaceKHR surface{};
    VkPhysicalDevice physicalDevice;
    PhysicalDeviceInfo physicalDeviceInfo;
    VkDevice device{};
    VkQueue graphicsQueue{};
    VkQueue presentationQueue{};
    SwapchainResources swapchainResources;
    VkCommandPool commandPool{};
    VkImage textureImage{};
    VkDeviceMemory textureImageMemory{};
    VkImageView textureImageView{};
    VkSampler textureSampler{};
    VkDescriptorSetLayout descriptorSetLayout{};
    VkDescriptorPool descriptorPool{};
    Array<VkDescriptorSet, maxFramesInFlight> descriptorSets;
    VkPipelineLayout pipelineLayout{};
    VkPipeline graphicsPipeline{};
    Array<BufferResources, maxFramesInFlight> quadDataBuffers;
    Array<VkCommandBuffer, maxFramesInFlight> commandBuffers;

    SynchronizationResources synchronizationResources;
    std::uint32_t currentFrameIndex{};
};
export inline RenderEngine renderEngine;

/*
 * Implementation of QuadReference methods
 */
template <QuadLayer layer>
QuadReference<layer>::~QuadReference()
{
    if (index != -1)
        clear();
}
template <QuadLayer layer>
QuadReference<layer>& QuadReference<layer>::operator=(QuadReference&& rhs) noexcept
{
    auto& references = quadPool.getStorage<layer>().references;

    // Update reference pointers in quad pool
    if (index != -1)
        references[index] = &rhs;
    if (rhs.index != -1)
        references[rhs.index] = this;

    std::swap(index, rhs.index);
    return *this;
}
template <QuadLayer layer>
void QuadReference<layer>::setPosition(float positionX, float positionY) const
{
    logger.extraAssert(index != -1, "Set position of invalid quad reference");

    auto& data = quadPool.getStorage<layer>().data;
    data[index].setPosition(positionX, positionY);
}
template <QuadLayer layer>
void QuadReference<layer>::setGlyph(std::uint8_t glyph) const
{
    logger.extraAssert(index != -1, "Set glyph of invalid quad reference");

    auto& data = quadPool.getStorage<layer>().data;
    data[index].setGlyph(glyph);
}
template <QuadLayer layer>
void QuadReference<layer>::setColor(std::uint32_t packedColor) const
{
    logger.extraAssert(index != -1, "Set color of invalid quad reference");

    auto& data = quadPool.getStorage<layer>().data;
    data[index].setColor(packedColor);
}
template <QuadLayer layer>
void QuadReference<layer>::setBackgroundColor(std::uint32_t packedColor) const
{
    logger.extraAssert(index != -1, "Set background color of invalid quad reference");

    auto& data = quadPool.getStorage<layer>().data;
    data[index].setBackgroundColor(packedColor);
}
template <QuadLayer layer>
template <IsSameType<QuadData> T>
void QuadReference<layer>::init(T&& quadData)
{
    logger.extraAssert(index == -1, "QuadReference::init() - initialized already existing reference");

    auto& [data, references] = quadPool.getStorage<layer>();

    logger.extraAssert(data.getSize() < data.getCapacity(), "QuadReference::init() - quad pool was full");

    // Append reference to the end of the quad pool
    index = (std::int64_t)data.getSize();
    data.emplaceBack(std::forward<T>(quadData));
    references.emplaceBack(this);
}
template <QuadLayer layer>
void QuadReference<layer>::clear()
{
    logger.extraAssert(index != -1, "Clearing invalid QuadReference");

    auto& [data, references] = quadPool.getStorage<layer>();

    // Copy last element in place of deleted and pop
    data[index] = data.getBack();
    data.popBack();

    // Update the reference index
    references[index] = references.getBack();
    references[index]->index = index;
    references.popBack();

    index = -1;
}
