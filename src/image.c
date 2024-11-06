#include <stdio.h>

static uint32_t Utils_GetVulkanMemoryType(VkMemoryPropertyFlags properties, uint32_t typeBits) {
    VkPhysicalDeviceMemoryProperties prop;
    vkGetPhysicalDeviceMemoryProperties(Application_GetPhysicalDevice(), &prop);
    for (uint32_t i = 0; i < prop.memoryTypeCount; i++) {
        if ((prop.memoryTypes[i].propertyFlags & properties) == properties && typeBits & (1 << i))
            return i;
    }
    return 0xffffffff;  // No suitable memory type found
}

static uint32_t Utils_BytesPerPixel(ImageFormat format) {
    switch (format) {
        case IMAGE_FORMAT_RGBA:    return 4;
        case IMAGE_FORMAT_RGBA32F: return 16;
        default:                   return 0;
    }
}

static VkFormat Utils_WalnutFormatToVulkanFormat(ImageFormat format) {
    switch (format) {
        case IMAGE_FORMAT_RGBA:    return VK_FORMAT_R8G8B8A8_UNORM;
        case IMAGE_FORMAT_RGBA32F: return VK_FORMAT_R32G32B32A32_SFLOAT;
        default:                   return VK_FORMAT_UNDEFINED;
    }
}

Image* Image_Create(const char* path) {
    Image* img = (Image*)malloc(sizeof(Image));
    img->filepath = strdup(path);

    int width, height, channels;
    uint8_t* data = NULL;

    if (stbi_is_hdr(img->filepath)) {
        data = (uint8_t*)stbi_loadf(img->filepath, &width, &height, &channels, 4);
        img->format = IMAGE_FORMAT_RGBA32F;
    } else {
        data = stbi_load(img->filepath, &width, &height, &channels, 4);
        img->format = IMAGE_FORMAT_RGBA;
    }

    img->width = width;
    img->height = height;
    
    Image_AllocateMemory(img, img->width * img->height * Utils_BytesPerPixel(img->format));
    Image_SetData(img, data);
    stbi_image_free(data);

    return img;
}

void Image_Destroy(Image* img) {
    if (!img) return;
    Image_Release(img);
    free(img->filepath);
    free(img);
}

void Image_AllocateMemory(Image* img, uint64_t size) {
    VkDevice device = Application_GetDevice();

    VkResult err;
    VkFormat vulkanFormat = Utils_WalnutFormatToVulkanFormat(img->format);

    VkImageCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    info.imageType = VK_IMAGE_TYPE_2D;
    info.format = vulkanFormat;
    info.extent.width = img->width;
    info.extent.height = img->height;
    info.extent.depth = 1;
    info.mipLevels = 1;
    info.arrayLayers = 1;
    info.samples = VK_SAMPLE_COUNT_1_BIT;
    info.tiling = VK_IMAGE_TILING_OPTIMAL;
    info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    err = vkCreateImage(device, &info, NULL, &img->image);
    check_vk_result(err);

    VkMemoryRequirements req;
    vkGetImageMemoryRequirements(device, img->image, &req);

    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = req.size;
    alloc_info.memoryTypeIndex = Utils_GetVulkanMemoryType(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, req.memoryTypeBits);
    err = vkAllocateMemory(device, &alloc_info, NULL, &img->memory);
    check_vk_result(err);
    err = vkBindImageMemory(device, img->image, img->memory, 0);
    check_vk_result(err);

    VkImageViewCreateInfo view_info = {};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = img->image;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = vulkanFormat;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.layerCount = 1;
    err = vkCreateImageView(device, &view_info, NULL, &img->imageView);
    check_vk_result(err);

    VkSamplerCreateInfo sampler_info = {};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = VK_FILTER_LINEAR;
    sampler_info.minFilter = VK_FILTER_LINEAR;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    err = vkCreateSampler(device, &sampler_info, NULL, &img->sampler);
    check_vk_result(err);

    img->descriptorSet = (VkDescriptorSet)ImGui_ImplVulkan_AddTexture(img->sampler, img->imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void Image_Release(Image* img) {
    VkDevice device = Application_GetDevice();
    vkDestroySampler(device, img->sampler, NULL);
    vkDestroyImageView(device, img->imageView, NULL);
    vkDestroyImage(device, img->image, NULL);
    vkFreeMemory(device, img->memory, NULL);
    vkDestroyBuffer(device, img->stagingBuffer, NULL);
    vkFreeMemory(device, img->stagingBufferMemory, NULL);
}

void Image_SetData(Image* img, const void* data) {
    VkDevice device = Application_GetDevice();
    size_t upload_size = img->width * img->height * Utils_BytesPerPixel(img->format);

    VkResult err;
    if (!img->stagingBuffer) {
        VkBufferCreateInfo buffer_info = {};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = upload_size;
        buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        err = vkCreateBuffer(device, &buffer_info, NULL, &img->stagingBuffer);
        check_vk_result(err);

        VkMemoryRequirements req;
        vkGetBufferMemoryRequirements(device, img->stagingBuffer, &req);
        VkMemoryAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = req.size;
        alloc_info.memoryTypeIndex = Utils_GetVulkanMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits);
        err = vkAllocateMemory(device, &alloc_info, NULL, &img->stagingBufferMemory);
        check_vk_result(err);
        err = vkBindBufferMemory(device, img->stagingBuffer, img->stagingBufferMemory, 0);
        check_vk_result(err);
    }

    char* map = NULL;
    err = vkMapMemory(device, img->stagingBufferMemory, 0, upload_size, 0, (void**)(&map));
    check_vk_result(err);
    memcpy(map, data, upload_size);

    VkMappedMemoryRange range = {};
    range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    range.memory = img->stagingBufferMemory;
    range.size = upload_size;
    err = vkFlushMappedMemoryRanges(device, 1, &range);
    check_vk_result(err);
    vkUnmapMemory(device, img->stagingBufferMemory);
}
