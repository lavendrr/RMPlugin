#include "plugin.hpp"


struct RMDelay : Module {
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
		__INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		__OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	// Fixed maximum size buffer (supports up to ~192kHz * 5)
	rack::dsp::RingBuffer<float, 960000> ringBuffer;
	size_t actualBufferSize = 0;
	
	RMDelay() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(__INPUT, "");
		configOutput(__OUTPUT, "");
		
		// Initialize the actual buffer size based on current sample rate
		updateBufferSize();
	}
	
	// Custom method to update buffer size
	void updateBufferSize() {
		actualBufferSize = static_cast<size_t>(APP->engine->getSampleRate() * 5.0f);
	}
	
	// Override to handle sample rate changes
	void onSampleRateChange() override {
		updateBufferSize();
	}
	
	void onReset() override {
		ringBuffer.clear();
	}
	
	void process(const ProcessArgs& args) override {
		// Get input
		float input = inputs[__INPUT].getVoltage();
		
		// Push to buffer, respecting the virtual size limit
		if (ringBuffer.size() >= actualBufferSize) {
			// Buffer is "full" at our virtual size, so shift before pushing
			ringBuffer.shift();
		}
		ringBuffer.push(input);
		
		// Output the delayed signal (from 5 seconds ago)
		float output = 0.0f;
		if (ringBuffer.size() >= actualBufferSize) {
			// We have enough samples for the full delay
			size_t delayPos = ringBuffer.start + ringBuffer.size() - actualBufferSize;
			output = ringBuffer.data[delayPos % 960000]; // Use the fixed buffer size
		}
		
		outputs[__OUTPUT].setVoltage(output);
	}
};


struct RMDelayWidget : ModuleWidget {
	RMDelayWidget(RMDelay* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/RMDelay.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(20.32, 65.384)), module, RMDelay::__INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(20.32, 101.396)), module, RMDelay::__OUTPUT));
	}
};


Model* modelRMDelay = createModel<RMDelay, RMDelayWidget>("RMDelay");
