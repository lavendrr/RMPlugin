#include "plugin.hpp"


struct RMMute : Module {
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
		__INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	RMMute() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(__INPUT, "");
	}

	void process(const ProcessArgs& args) override {
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

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(126.199, 88.093)), module, RMMute::__INPUT));
	}
};


Model* modelRMMute = createModel<RMMute, RMMuteWidget>("RMMute");