#include "Renderer.hpp"
#include "App/Application.hpp"
#include "Core/Assets/ShaderModule.hpp"
#include "Core/Presentation/SwapChain.hpp"
#include "vulkan/vulkan.hpp"
#include <Core/Presentation/Window.hpp>
#include <vector>
#include <iostream>

using namespace pgr::Core::Rendering;

/* Shorthands */
using App = pgr::App::Application;

Renderer::Renderer(const Presentation::Window& target) : mSwapChain(target) {

    /* For now, so it works */
    Assets::ShaderModule vertModule = Assets::ShaderModule("sample.vert.spv");
    Assets::ShaderModule fragModule = Assets::ShaderModule("sample.frag.spv");

    vk::PipelineShaderStageCreateInfo vertStageInfo = vk::PipelineShaderStageCreateInfo()
        .setStage(vk::ShaderStageFlagBits::eVertex)
        .setModule(vertModule)
        .setPName("main");

    vk::PipelineShaderStageCreateInfo fragStageInfo = vk::PipelineShaderStageCreateInfo()
        .setStage(vk::ShaderStageFlagBits::eFragment)
        .setModule(fragModule)
        .setPName("main");

    /* TODO: PVP */
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo = vk::PipelineVertexInputStateCreateInfo()
        .setVertexAttributeDescriptionCount(0)
        .setVertexBindingDescriptionCount(0);

    /* Why the fuck vulkan? */
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly = vk::PipelineInputAssemblyStateCreateInfo()
        .setTopology(vk::PrimitiveTopology::eTriangleList)
        .setPrimitiveRestartEnable(false);


    vk::Extent2D fbSize = mSwapChain.FrameBufferSize();
    vk::Viewport viewport = vk::Viewport()
        .setX(0).setY(0)
        .setWidth(static_cast<float>(fbSize.width))
        .setHeight(static_cast<float>(fbSize.height))
        .setMinDepth(0.0f).setMaxDepth(1.0f);

    vk::Rect2D scissor = vk::Rect2D()
        .setOffset((vk::Offset2D) { 0,  0})
        .setExtent(fbSize);


    vk::PipelineViewportStateCreateInfo viewportState = vk::PipelineViewportStateCreateInfo()
        .setViewportCount(1).setPViewports(&viewport)
        .setScissorCount(1).setPScissors(&scissor);

    vk::PipelineRasterizationStateCreateInfo rasterizer = vk::PipelineRasterizationStateCreateInfo()
        .setDepthClampEnable(false)
        .setRasterizerDiscardEnable(false)
        .setPolygonMode(vk::PolygonMode::eFill)
        .setLineWidth(1.0f)
        .setCullMode(vk::CullModeFlagBits::eBack)
        .setFrontFace(vk::FrontFace::eClockwise)
        .setDepthBiasEnable(false);

    vk::PipelineMultisampleStateCreateInfo multisampling = vk::PipelineMultisampleStateCreateInfo()
        .setSampleShadingEnable(false)
        .setRasterizationSamples(vk::SampleCountFlagBits::e1);

    vk::PipelineColorBlendAttachmentState colorBlendAttachment = vk::PipelineColorBlendAttachmentState()
        .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
        .setBlendEnable(false);

    vk::PipelineColorBlendStateCreateInfo colorBlending = vk::PipelineColorBlendStateCreateInfo() /* UUUUGH THIS IS WHY THEY MADE AI!!! */
        .setLogicOpEnable(false)
        .setAttachmentCount(1)
        .setPAttachments(&colorBlendAttachment)
        .setBlendConstants(std::array<float, 4> {0, 0, 0, 0});

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo = vk::PipelineLayoutCreateInfo()
        .setSetLayoutCount(0)
        .setPushConstantRangeCount(0);

    vk::AttachmentDescription colorAttachment = vk::AttachmentDescription()
        .setFormat(mSwapChain.FrameBufferFormat())
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

    vk::AttachmentReference colorAttachmentRef = vk::AttachmentReference()
        .setAttachment(0)
        .setLayout(vk::ImageLayout::eColorAttachmentOptimal);

    vk::SubpassDescription subpass = vk::SubpassDescription()
        .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
        .setColorAttachmentCount(1)
        .setPColorAttachments(&colorAttachmentRef);

    vk::SubpassDependency dependency = vk::SubpassDependency()
        .setSrcSubpass(vk::SubpassExternal)
        .setDstSubpass(0)
        .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
        .setSrcAccessMask(vk::AccessFlagBits::eNone)
        .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

    vk::RenderPassCreateInfo renderPassInfo = vk::RenderPassCreateInfo()
        .setAttachmentCount(1)
        .setPAttachments(&colorAttachment)
        .setSubpassCount(1)
        .setPSubpasses(&subpass)
        .setDependencyCount(1)
        .setPDependencies(&dependency);

    mLayout = static_cast<vk::Device>(::App::UsedDevice()).createPipelineLayout(pipelineLayoutInfo);
    mRenderPass = static_cast<vk::Device>(::App::UsedDevice()).createRenderPass(renderPassInfo);

    std::array<const vk::PipelineShaderStageCreateInfo, 2>  shaderStages { vertStageInfo, fragStageInfo};
    vk::GraphicsPipelineCreateInfo pipelineInfo = vk::GraphicsPipelineCreateInfo()
        .setStages(shaderStages)
        .setPVertexInputState(&vertexInputInfo)
        .setPInputAssemblyState(&inputAssembly)
        .setPViewportState(&viewportState)
        .setPRasterizationState(&rasterizer)
        .setPMultisampleState(&multisampling)
        .setPColorBlendState(&colorBlending)
        .setPDynamicState(nullptr)
        .setLayout(mLayout)
        .setRenderPass(mRenderPass)
        .setSubpass(0)
        .setBasePipelineHandle(nullptr);

    const auto& pipelines = static_cast<vk::Device>(::App::UsedDevice()).createGraphicsPipeline(nullptr, pipelineInfo);
    mPipeline = pipelines.value;

    /* Framebuffers */
    for (const auto& image : mSwapChain.SwapChainImages()) {

        std::cout << image << std::endl;

        std::array<vk::ImageView, 1> attachments { image };

        vk::FramebufferCreateInfo framebufferInfo = vk::FramebufferCreateInfo()
            .setRenderPass(mRenderPass)
            .setAttachments(attachments)
            .setWidth(fbSize.width)
            .setHeight(fbSize.height)
            .setLayers(1);

        vk::Framebuffer fb = static_cast<vk::Device>(::App::UsedDevice()).createFramebuffer(framebufferInfo);
        mFrameBuffers.emplace_back(fb);
    }

    vk::CommandPoolCreateInfo commandPoolInfo = vk::CommandPoolCreateInfo()
        .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
        .setQueueFamilyIndex(::App::UsedDevice().GraphicsQueueFamily());

    mCommandPool = static_cast<vk::Device>(::App::UsedDevice()).createCommandPool(commandPoolInfo);

    vk::CommandBufferAllocateInfo commandBufferInfo = vk::CommandBufferAllocateInfo()
        .setCommandBufferCount(1)
        .setCommandPool(mCommandPool)
        .setLevel(vk::CommandBufferLevel::ePrimary);

    const auto& commandBuffers = static_cast<vk::Device>(::App::UsedDevice()).allocateCommandBuffers(commandBufferInfo);
    mCommandBuffer = commandBuffers[0];

    vk::FenceCreateInfo fenceInfo = vk::FenceCreateInfo()
        .setFlags(vk::FenceCreateFlagBits::eSignaled);
    mFrameDrawnFence = static_cast<vk::Device>(::App::UsedDevice()).createFence(fenceInfo);
    mRenderingDoneSemaphore = static_cast<vk::Device>(::App::UsedDevice()).createSemaphore((vk::SemaphoreCreateInfo) {});
}

Renderer::~Renderer() {

    ::App::UsedDevice().WaitToFinish();
    for (const auto& fb : mFrameBuffers)
            static_cast<vk::Device>(::App::UsedDevice()).destroyFramebuffer(fb);

    static_cast<vk::Device>(::App::UsedDevice()).destroySemaphore(mRenderingDoneSemaphore);
    static_cast<vk::Device>(::App::UsedDevice()).destroyFence(mFrameDrawnFence);


    static_cast<vk::Device>(::App::UsedDevice()).destroyPipeline(mPipeline);
    static_cast<vk::Device>(::App::UsedDevice()).destroyRenderPass(mRenderPass);
    static_cast<vk::Device>(::App::UsedDevice()).destroyPipelineLayout(mLayout);
    static_cast<vk::Device>(::App::UsedDevice()).destroyCommandPool(mCommandPool);
}


void Renderer::Dispatch() {

    auto result = static_cast<vk::Device>(::App::UsedDevice()).waitForFences(1, &mFrameDrawnFence, true, 1ull << 63);
    result = static_cast<vk::Device>(::App::UsedDevice()).resetFences(1, &mFrameDrawnFence);

    uint32_t usedFramebuffer = mSwapChain.RequestNextImage();
    mCommandBuffer.reset();

    vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo();
    mCommandBuffer.begin(beginInfo);

    vk::ClearValue clearColor = vk::ClearValue()
        .setColor(vk::ClearColorValue()
            .setFloat32(std::array<float, 4> { 1.0f, 0.25f, 0.75f, 1.0f }));

    vk::RenderPassBeginInfo passBeginInfo = vk::RenderPassBeginInfo()
        .setRenderPass(mRenderPass)
        .setFramebuffer(mFrameBuffers[usedFramebuffer])
        .setRenderArea((vk::Rect2D) { (vk::Offset2D) {0, 0}, mSwapChain.FrameBufferSize() })
        .setClearValueCount(1)
        .setPClearValues(&clearColor);


    mCommandBuffer.beginRenderPass(passBeginInfo, vk::SubpassContents::eInline);
    mCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);
    mCommandBuffer.draw(3, 1, 0, 0);

    mCommandBuffer.endRenderPass();
    mCommandBuffer.end();


    std::array<vk::Semaphore, 1> waitSemaphores { mSwapChain.ImageRequestSemaphore() },
                                 signalSemaphores { mRenderingDoneSemaphore };
    std::array<vk::PipelineStageFlags, 1> waitStages { vk::PipelineStageFlagBits::eColorAttachmentOutput };

    vk::SubmitInfo submitInfo = vk::SubmitInfo()
        .setWaitSemaphores(waitSemaphores)
        .setWaitDstStageMask(waitStages)
        .setCommandBufferCount(1)
        .setPCommandBuffers(&mCommandBuffer)
        .setSignalSemaphores(signalSemaphores);

    ::App::UsedDevice().GraphicsQueue().submit(submitInfo, mFrameDrawnFence);

    vk::SwapchainKHR swapchain = mSwapChain;
    vk::PresentInfoKHR presentInfo = vk::PresentInfoKHR()
        .setWaitSemaphores(signalSemaphores)
        .setSwapchainCount(1)
        .setPSwapchains(&swapchain)
        .setPImageIndices(&usedFramebuffer);

    result = ::App::UsedDevice().PresentQueue().presentKHR(presentInfo);
}


/*
void renderer::request_draw(const observer_ptr<mesh_instance>& mesh_instance, const glm::mat4x4& transform) {

    /* If mesh is invalid, there is no point in drawing it 
    if (!mesh_instance.valid())
        return;

    /* Create draw request 
    draw_request req = {
        draw_request::draw_command{
            mesh_instance->get_mesh()->element_count(),
            1, /* No instancing RN 
            mesh_instance->get_mesh()->first_index(),
            static_cast<int>(mesh_instance->get_mesh()->first_vertex()),
            0 /* No instancing RN
        },
        draw_request::object_data{
            transform,
            glm::transpose(glm::inverse(transform)),
            mesh_instance->get_material().uv_mat(),
            mesh_instance->get_material().material_index()
        },
        mesh_instance->get_material().transparent(),
        mesh_instance->get_material().shader_stages()  
    };

    /* Insert into list 
    m_enqueued_objects.insert(std::move(req), 
        static_cast<GLuint>(*mesh_instance->get_material().shader_stages().at(shader_stage::c_known_stage_types[1])), 
        static_cast<GLuint>(*mesh_instance->get_material().shader_stages().at(shader_stage::c_known_stage_types[1]))
    );
}

/* This... this is gonna be a big one 
void renderer::draw_scene() {

    //===============================
    // SETUP - Prepare rendering
    //===============================

    /* No valid camera bound or nothing to draw, end the draw function 
    if (!m_active_camera.valid() || m_enqueued_objects.empty())
        return;

    /* Update view matrix uniform 
    glNamedBufferSubData(
        m_active_camera->camera_data(), 
        sizeof(mat4x4), sizeof(mat4x4), 
        glm::value_ptr(m_active_camera->view())
    );

    /* Prepare object data for drawing 
    std::vector<render_pass> draw_passes;
    m_prepare_drawing(draw_passes);

    /* Update light data  & prepare for drawing 
    glNamedBufferData(m_light_storage, m_lights.size() * sizeof(m_lights[0]), m_lights.data(), GL_DYNAMIC_DRAW);
    glBindProgramPipeline(m_pipeline);
    glBindVertexArray(m_models_vao);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_draw_cmd_queue);
 
    GLuint objects_drawn = 0;
    auto pass = draw_passes.begin();

    /* Clean the fbo's depth 
    glClearNamedFramebufferfi(m_default_target.fbo, GL_DEPTH_STENCIL, 0, 1.0f, 0.0f);

    //===============================
    // PASS 1 - Opaque objects
    //===============================

    /* Bind default FBO 
    glBindFramebuffer(GL_FRAMEBUFFER, m_default_target.fbo);

    /* Enable only opaque attachment for drawing and clear it 
    glNamedFramebufferDrawBuffers(m_default_target.fbo, g_opaque_attachments.size(), g_opaque_attachments.data());
    glClearNamedFramebufferfv(m_default_target.fbo, GL_COLOR, 0, value_ptr(glm::vec4(0.0)));

    /* Set appropriate OpenGL state 
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunci(0, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Itterate over draw passes and draw them 
    for (; pass != draw_passes.end(); ++pass) {
        
        /* Upon reaching the first transparent pass, end this step 
        if (pass->transparent)
            break;

        /* Bind shader delta 
        for (const auto& stage : pass->shader_delta)
            attach_stage(stage);

        /* Set uniforms correctly - Only TWO (engine) uniforms per shader! 
        set_uniform("draw_id_offset", objects_drawn, GL_VERTEX_SHADER_BIT);
        set_uniform("light_count", static_cast<uint>(m_lights.size()), GL_FRAGMENT_SHADER_BIT);
        set_uniform("global_time", engine_runtime::instance()->global_clock());

        /* Draw! 
        glMultiDrawElementsIndirect(
            GL_TRIANGLES, GL_UNSIGNED_INT, 
            reinterpret_cast<void*>(objects_drawn * sizeof(draw_request::draw_command)), 
            pass->object_count, 0
        );

        objects_drawn += pass->object_count;
    }

    //===============================
    // PASS 2 - Transparent objects
    //===============================

    /* Enable only transparent attachments for drawing and clear them 
    glNamedFramebufferDrawBuffers(m_default_target.fbo, g_transparent_attachments.size(), g_transparent_attachments.data());
    glClearNamedFramebufferfv(m_default_target.fbo, GL_COLOR, 0, value_ptr(glm::vec4(0.0)));
    glClearNamedFramebufferfv(m_default_target.fbo, GL_COLOR, 1, value_ptr(glm::vec4(1.0))); 

    /* Set appropriate OpenGL state 
    glDepthMask(GL_FALSE);
    glBlendFunci(0, GL_ONE, GL_ONE);
    glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);

    /* Draw the rest of the passes 
    for (; pass != draw_passes.end(); ++pass) {
    
        /* Bind shader delta 
        for (const auto& stage : pass->shader_delta)
            attach_stage(stage);

        /* Set uniforms correctly - Only TWO (engine) uniforms per shader! 
        set_uniform("draw_id_offset", objects_drawn, GL_VERTEX_SHADER_BIT);
        set_uniform("light_count", static_cast<uint>(m_lights.size()), GL_FRAGMENT_SHADER_BIT);
        set_uniform("global_time", engine_runtime::instance()->global_clock());

        /* Draw! 
        glMultiDrawElementsIndirect(
            GL_TRIANGLES, GL_UNSIGNED_INT, 
            reinterpret_cast<void*>(objects_drawn * sizeof(draw_request::draw_command)), 
            pass->object_count, 0
        );

        objects_drawn += pass->object_count;
    }

    //===============================
    // INTERMEZZO - Skybox
    //===============================
    
    /* Set opaque attachment up for drawing 
    
    glNamedFramebufferDrawBuffers(m_default_target.fbo, g_opaque_attachments.size(), g_opaque_attachments.data());
    glDepthFunc(GL_LEQUAL);
    
    /* If the scene has skybox, draw it! 
    if (m_current_skybox) {
        
        attach_stage(m_skybox_vertex_shader);
        attach_stage(m_skybox_fragment_shader);

        glDrawArraysInstancedBaseInstance(
            GL_TRIANGLES, 
            m_skybox_first_vertex, 
            c_skybox_mesh.size(), 1, 0
        );
    }

    //===============================
    // PASS 3 - Blend pass
    //===============================

    /* If no postprocess passes will follow, choose back buffer directly 
    /* Otherwise, choose the postprocess FBO 
    glBindFramebuffer(
        GL_FRAMEBUFFER, 
        m_postprocess_passes.size() > 0 ? m_postprocess_targets[0].fbo : 0
    );

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_ALWAYS);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* If no object nor skybox were drawn, end the frame now 
    if (objects_drawn == 0 && !m_current_skybox) {
        m_end_draw();
        return;
    }
    
    /* Attach default FBO's attachments as textures 
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_default_target.opaque_target);   
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_default_target.accum_target);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_default_target.reveal_target);
    
    /* Setup shaders 
    attach_stage(m_quad_vertex_shader);
    attach_stage(m_combination_shader);
    
    /* Set uniforms 
    set_uniform("opaque_target", 0);
    set_uniform("accum_target", 1);
    set_uniform("reveal_target", 2);
    
    /* Draw! 
    glDrawArraysInstancedBaseInstance(
        GL_TRIANGLES, 
        m_quad_first_vertex, 
        c_quad_mesh.size(), 1, 0
    );
    
    //===============================
    // PASS 4 - Post-processing
    //===============================

    /* Definitions of "front" and "back" buffers - allows having just two buffers for the 
    /* entirety of postprocessing 
    GLuint front = 0, 
           back = 1;

    for (auto pp_pass = m_postprocess_passes.begin(); pp_pass != m_postprocess_passes.end(); ++pp_pass) {

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_postprocess_targets[0].color_target);   
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_default_target.depth_stencil_target);
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_DEPTH_COMPONENT);


        /* Setup shaders 
        attach_stage(*pp_pass);
        
        /* Set uniforms 
        set_uniform("color_target", 0);
        set_uniform("depth_target", 1);
        
        /* If this is the last postprocessing pass, bind back buffer 
        glBindFramebuffer(
            GL_FRAMEBUFFER, 
            utils::is_last_in_container(m_postprocess_passes, pp_pass) ? 0 : m_postprocess_targets[0].fbo
        );
        
        /* Draw! 
        glDrawArraysInstancedBaseInstance(
            GL_TRIANGLES, 
            m_quad_first_vertex, 
            c_quad_mesh.size(), 1, 0
        );
        
        /* Swap buffers 
        front = back;
        back ^= 1;
    }

    //===============================
    // END - next-frame preparation
    //===============================
    m_end_draw();
}

void renderer::m_prepare_drawing(vector<render_pass>& draw_passes) {

    /* Create command queues 
    vector<draw_request::draw_command> commands;
    vector<draw_request::object_data> object_data;

    /* Split objects to opaque and transparent 
    auto first_transparent = stable_partition(
        m_enqueued_objects.begin(), m_enqueued_objects.end(), 
        [](draw_request req) { return !req.transparent; }
    );

    /* Currently bound shaders 
    shader_map current_shaders;
    bool is_transparent = false;
    
    /* This waay before than the code before 
    auto object = m_enqueued_objects.begin();
    while (object != m_enqueued_objects.end()) {
        
        /* Prepare new pass 
        GLuint objects_in_pass = 0;
        shader_list shader_delta;

        /* Prepare space for shaders - by default vert and frag 
        draw_passes.reserve(2); 
        
        /* Process shader delta 
        for (const auto& [type, stage] : object->used_stages) {

            if (auto iter = current_shaders.find(type); iter == current_shaders.end() || iter->second != stage) {
                current_shaders[type] = stage;
                shader_delta.push_back(stage);
            }
        }

        /* Process objects 
        for (; object != m_enqueued_objects.end(); ++object) {

            /* Transparent passes start 
            if (!is_transparent && object == first_transparent) 
                break;

            /* Shader missmatch 
            if (m_has_shader_missmatch(object->used_stages, current_shaders))
                break;

            for (const auto& [key, stage] : object->used_stages) {
                if (auto iter = current_shaders.find(key); iter == current_shaders.end() || iter->second != stage)
                    break;
            }

            /* Push data to queues 
            commands.push_back(std::move(object->command));
            object_data.push_back(std::move(object->data));
            objects_in_pass++;
        }

        /* Save pass 
        if (objects_in_pass > 0)
            draw_passes.emplace_back(render_pass{is_transparent, objects_in_pass, shader_delta});

        if (object == first_transparent)
            is_transparent = true;
    }

    glNamedBufferData(m_draw_cmd_queue, commands.size() * sizeof(commands[0]), commands.data(), GL_DYNAMIC_DRAW);
    glNamedBufferData(m_object_storage, object_data.size() * sizeof(object_data[0]), object_data.data(), GL_DYNAMIC_DRAW);
}
*/

