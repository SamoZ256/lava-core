#include "metal/lvcore/core/image.hpp"

#include <string>
#include <filesystem>

#import <MetalKit/MetalKit.h>

#include "stb/stb_image.h"
//#include "gli/gli.hpp"

#include "metal/lvcore/core/core.hpp"

#include "metal/lvcore/core/device.hpp"
#include "metal/lvcore/core/swap_chain.hpp"
#include "metal/lvcore/core/buffer.hpp"

namespace lv {

id Metal_Image::blitComputeLibrary = nil;
id Metal_Image::blitComputeFunction = nil;
id Metal_Image::blitComputePipelineState = nil;

Metal_Image::Metal_Image(Metal_ImageCreateInfo createInfo) {
    _frameCount = (createInfo.frameCount == 0 ? g_metal_swapChain->maxFramesInFlight() : createInfo.frameCount);

    _format = createInfo.format;
    _width = createInfo.width;
    _height = createInfo.height;
    _layerCount = createInfo.layerCount;
    _mipCount = createInfo.mipCount;

    MTLPixelFormat mtlPixelFormat;
    GET_MTL_PIXEL_FORMAT(_format, mtlPixelFormat);
    MTLTextureType mtlTextureType;
    GET_MTL_TEXTURE_TYPE(createInfo.imageType, mtlTextureType);
    MTLStorageMode mtlStorageMode;
    GET_MTL_STORAGE_MODE(createInfo.memoryType, mtlStorageMode);
    MTLTextureUsage mtlTextureUsage = metal::getMTLTextureUsage(createInfo.usage); //TODO: remove the metal namespace
    
    if (createInfo.imageType == ImageType::Cube || createInfo.imageType == ImageType::CubeArray)
        _layersPerLayer = 6;

    MTLTextureDescriptor* textureDesc = [[MTLTextureDescriptor alloc] init];
    textureDesc.width = _width;
    textureDesc.height = _height;
    textureDesc.pixelFormat = mtlPixelFormat;
    textureDesc.textureType = mtlTextureType;
    textureDesc.storageMode = mtlStorageMode;
    textureDesc.usage = mtlTextureUsage;
    textureDesc.arrayLength = _layerCount;
    textureDesc.mipmapLevelCount = _mipCount;

    images.resize(_frameCount);
    for (uint8_t i = 0; i < _frameCount; i++)
        images[i] = [g_metal_device->device() newTextureWithDescriptor:textureDesc];

    [textureDesc release];
}

Metal_Image::Metal_Image(Metal_ImageLoadInfo loadInfo, Metal_CommandBuffer* commandBuffer) {
    _frameCount = 1;

    std::string strFilename = std::string(loadInfo.filename);
    std::replace(strFilename.begin(), strFilename.end(), '\\', '/');
    strFilename = std::filesystem::absolute(std::filesystem::path(strFilename));
    /*
    while (true) {
        size_t pos = strFilename.find("/..");
        if (pos == std::string::npos)
            break;
        size_t removePos = strFilename.find_last_of("/", pos - 4);
        strFilename.replace(removePos, pos - removePos + 3, "");
    }
    */
    /*
    std::string extension = strFilename.substr(strFilename.find_last_of('.'));
    int aWidth, aHeight, nbChannels;
    void* data = (void*)stbi_load(strFilename.c_str(), &aWidth, &aHeight, &nbChannels, STBI_rgb_alpha);

    if (!data && extension == ".dds") {
        gli::texture image = gli::load("/Users/samuliak/Desktop/test.dds");
        auto extent = image.extent();
        if (!image.empty() && extent.x > 0 && extent.y > 0) {
            aWidth = extent.x;
            aHeight = extent.y;
            data = image.data();
            //uint8_t* srcData = (uint8_t*)image.data();
            //data = malloc(width * height);
            //
            //auto blockSize = 8;
            //for (auto y = 0; y < height / 4; ++y) {
            //    uint8_t* rowDest = (uint8_t*)data; // rowPitch is 0
            //    uint8_t* rowSrc = srcData + y * ((width / 4) * blockSize);
            //    for (auto x = 0; x < width / 4; ++x) {
            //        auto* pxDest = rowDest + x * blockSize;
            //        auto* pxSrc = rowSrc + x * blockSize; // 4x4 image block
            //        memcpy(pxDest, data, blockSize); // 64Bit per block
            //    }
            //}
        } else {
            std::cout << "Failed to read dds file" << std::endl;
        }
    }

    if (data) {
        if (bGenerateMipmaps)
            mipCount = std::max(std::max(ceil(log2(aWidth)), ceil(log2(aHeight))), 1.0);

        init(aWidth, aHeight);

        copyDataTo(0, data);
        
        if (extension != ".dds")
            stbi_image_free(data);

        if (bGenerateMipmaps) {
            Metal_CommandBuffer commandBuffer;
            commandBuffer.frameCount = 1;
            commandBuffer.init();
            commandBuffer.bind();
            generateMipmaps();
            commandBuffer.unbind();
            commandBuffer.submit();
            commandBuffer.destroy();
        }
    }
    else {
        init(1, 1);

        std::cout << "Failed to load image '" << strFilename << "'" << std::endl;
    }
    */

    NSError* error;
    MTKTextureLoader* loader = [[MTKTextureLoader alloc] initWithDevice:g_metal_device->device()];
    
    NSURL* url = [[NSURL alloc] initWithString:[NSString stringWithFormat:@"file://%s", strFilename.c_str()]];
    //std::cout << "URL: " << [[url path] UTF8String] << std::endl;
    images.resize(1);
    images[0] = [loader newTextureWithContentsOfURL:url options:@{
        MTKTextureLoaderOptionGenerateMipmaps : [NSNumber numberWithBool:loadInfo.generateMipmaps],
        MTKTextureLoaderOptionSRGB : [NSNumber numberWithBool:loadInfo.isSRGB]
    } error:&error];

    _width = ((id<MTLTexture>)images[0]).width;
    _height = ((id<MTLTexture>)images[0]).height;
    
    if (!images[0])
        NSLog(@"Error creating the texture from %@: %@", url.absoluteString, error.localizedDescription);
}

Metal_Image::Metal_Image(Metal_ImageViewCreateInfo viewCreateInfo) {
    _frameCount = viewCreateInfo.image->frameCount();
    _isOriginal = false;
    _width = viewCreateInfo.image->width();
    _height = viewCreateInfo.image->height();
    _format = viewCreateInfo.image->format();

    _baseLayer = viewCreateInfo.baseLayer;
    _layerCount = viewCreateInfo.layerCount;
    _baseMip = viewCreateInfo.baseMip;
    _mipCount = viewCreateInfo.mipCount;
    
    if (viewCreateInfo.viewType == ImageType::Cube || viewCreateInfo.viewType == ImageType::CubeArray)
        _layersPerLayer = 6;
    
    MTLPixelFormat mtlPixelFormat;
    GET_MTL_PIXEL_FORMAT(_format, mtlPixelFormat);
    MTLTextureType mtlTextureType;
    GET_MTL_TEXTURE_TYPE(viewCreateInfo.viewType, mtlTextureType);

    images.resize(_frameCount);
    for (uint8_t i = 0; i < _frameCount; i++) {
        images[i] = [images[i] newTextureViewWithPixelFormat:mtlPixelFormat
                                                          textureType:mtlTextureType
                                                               levels:NSMakeRange(_baseMip, _mipCount)
                                                               slices:NSMakeRange(_baseLayer, _layerCount)];
    }
}

Metal_Image::~Metal_Image() {
    if (_isOriginal) {
        for (uint8_t i = 0; i < _frameCount; i++)
            [images[i] release];
    }
}

Metal_ImageDescriptorInfo Metal_Image::descriptorInfo(uint32_t binding, DescriptorType descriptorType, ImageLayout imageLayout, int8_t frameOffset) {
    Metal_ImageDescriptorInfo info;
    info.images.resize(_frameCount);
    for (uint8_t i = 0; i < _frameCount; i++) {
        int8_t index = i + frameOffset;
        if (index < 0) index += _frameCount;
        else if (index >= _frameCount) index -= _frameCount;
        info.images[i] = images[index];
    }
    info.binding = binding;
    info.descriptorType = descriptorType;

    return info;
}
    
Metal_Image* Metal_Image::newImageView(lv::ImageType viewType, uint16_t baseLayer, uint16_t layerCount, uint16_t baseMip, uint16_t mipCount) {
    Metal_Image* newImage = new Metal_Image({
        .image = this,
        .viewType = viewType,
        .baseLayer = baseLayer,
        .layerCount = layerCount,
        .baseMip = baseMip,
        .mipCount = mipCount
    });

    return newImage;
}

} //namespace lv
