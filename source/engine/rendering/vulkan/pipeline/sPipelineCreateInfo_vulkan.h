#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

namespace df::vulkan
{
	struct sPipelineCreateInfo_vulkan
	{
		void setShaders( vk::ShaderModule _vertex, vk::ShaderModule _fragment );
		void setInputTopology( vk::PrimitiveTopology _topology, bool _primitive_restart_enable = false );
		void setpolygonMode( vk::PolygonMode _mode, float _line_width = 1 );
		void setCullMode( vk::CullModeFlags _cull_mode, vk::FrontFace _front_face );
		void setMultisamplingNone();

		void enableBlendingAdditive();
		void enableBlendingAlphablend();
		void disableBlending();

		void enableDepthtest( bool _depth_write_enable, vk::CompareOp _operation );
		void disableDepthtest();

		void setColorFormat( vk::Format _format );
		void setColorFormats( const std::vector< vk::Format >& _formats );
		void setDepthFormat( vk::Format _format );

		vk::PipelineRasterizationStateCreateInfo rasterizer{};
		vk::PipelineRenderingCreateInfo          render_info{};
		vk::PipelineDepthStencilStateCreateInfo  depth_stencil{};
		vk::PipelineMultisampleStateCreateInfo   multisampling{};
		vk::PipelineInputAssemblyStateCreateInfo input_assembly{};
		vk::PipelineColorBlendAttachmentState    color_blend_attachment{};
		std::vector< vk::Format >                color_attachment_formats;

		std::vector< vk::PipelineShaderStageCreateInfo >   shader_stages;
		std::vector< vk::PushConstantRange >               push_constant_ranges;
		std::vector< vk::DescriptorSetLayout >             descriptor_layouts;
		std::vector< vk::VertexInputBindingDescription >   vertex_input_binding;
		std::vector< vk::VertexInputAttributeDescription > vertex_input_attribute;

		std::string name;
	};
}
