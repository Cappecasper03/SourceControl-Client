#pragma once

#include <string>

#include "engine/misc/cTransform.h"
#include "engine/misc/Misc.h"

namespace df
{
	struct iRenderCallback;

	struct iAsset
	{
		DF_DISABLE_COPY_AND_MOVE( iAsset );

		explicit iAsset( std::string _name );
		virtual ~iAsset() = default;

		virtual void update( float /*_delta_time*/ = 0 ) {}
		virtual void render() {}

		const std::string name;
	};

	class iRenderAsset : public iAsset
	{
	public:
		DF_DISABLE_COPY_AND_MOVE( iRenderAsset );

		explicit iRenderAsset( std::string _name );
		~iRenderAsset() override;

		void update( float /*_delta_time*/ = 0 ) override { transform->update(); }
		void render() override = 0;

		cTransform*      transform;
		iRenderCallback* render_callback;
	};
}
