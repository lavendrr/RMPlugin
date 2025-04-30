#include "plugin.hpp"


struct RMMute : Module {
	enum ParamId {
		MUTE_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		AUDIO_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		AUDIO_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		MUTE_LIGHT,
		LIGHTS_LEN
	};

	dsp::BooleanTrigger muteTrigger;
	bool muted = false;

	RMMute() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configButton(MUTE_PARAM, "Mute");
		configInput(AUDIO_INPUT, "Audio");
		configOutput(AUDIO_OUTPUT, "Audio");
	}

	void onReset(const ResetEvent& e) override {
		Module::onReset(e);
		muted = false;
	}

	void process(const ProcessArgs& args) override {
		// Process mute button
		if (muteTrigger.process(params[MUTE_PARAM].getValue()))
			muted = !muted;

		// Process audio
		float input = inputs[AUDIO_INPUT].getVoltage();
		float output = muted ? 0.f : input;
		outputs[AUDIO_OUTPUT].setVoltage(output);

		// Update light
		lights[MUTE_LIGHT].setBrightnessSmooth(muted, args.sampleTime);
	}

	json_t* dataToJson() override {
		json_t* rootJ = json_object();
		json_object_set_new(rootJ, "muted", json_boolean(muted));
		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override {
		json_t* mutedJ = json_object_get(rootJ, "muted");
		if (mutedJ)
			muted = json_boolean_value(mutedJ);
	}
};


struct RMMuteWidget : ModuleWidget {
	RMMuteWidget(RMMute* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/RMMute.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(20.32, 65.384)), module, RMMute::AUDIO_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(20.32, 101.396)), module, RMMute::AUDIO_OUTPUT));

		// addChild(createWidgetCentered<Widget>(mm2px(Vec(20.32, 33.8))));
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<RedLight>>>(
			mm2px(Vec(20.32, 33.8)), module, RMMute::MUTE_PARAM, RMMute::MUTE_LIGHT));
	}
};


Model* modelRMMute = createModel<RMMute, RMMuteWidget>("RMMute");