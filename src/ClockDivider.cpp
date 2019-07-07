#include "dsp/digital.hpp"
#include "plugin.hpp"

struct ClockDivider : Module {
  enum ParamIds { NUM_PARAMS };
  enum InputIds { CLOCK_INPUT, NUM_INPUTS };
  enum OutputIds {
    DIV2_OUTPUT,
    DIV4_OUTPUT,
    DIV8_OUTPUT,
    DIV16_OUTPUT,
    NUM_OUTPUTS
  };
  enum LightIds { NUM_LIGHTS };

  int m_counter;
  dsp::SchmittTrigger m_trigger;
  dsp::PulseGenerator m_div2;
  dsp::PulseGenerator m_div4;
  dsp::PulseGenerator m_div8;
  dsp::PulseGenerator m_div16;

  ClockDivider() : m_counter(0) {
    config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
  }

  void process(const ProcessArgs &args) override {
    float clock = inputs[CLOCK_INPUT].getVoltage();
    if (m_trigger.process(rescale(clock, 0.1f, 2.f, 0.f, 1.f))) {
      m_counter++;

      if (m_counter > 16)
        m_counter = 0;
    }

    if (m_counter % 2 == 0)
      m_div2.trigger(1e-3);
    if (m_counter % 4 == 0)
      m_div4.trigger(1e-3);
    if (m_counter % 8 == 0)
      m_div8.trigger(1e-3);
    if (m_counter % 16 == 0)
      m_div16.trigger(1e-3);

    outputs[DIV2_OUTPUT].setVoltage(m_div2.process(args.sampleTime) ? 10.f : 0.f);
    outputs[DIV4_OUTPUT].setVoltage(m_div4.process(args.sampleTime) ? 10.f : 0.f);
    outputs[DIV8_OUTPUT].setVoltage(m_div8.process(args.sampleTime) ? 10.f : 0.f);
    outputs[DIV16_OUTPUT].setVoltage(m_div16.process(args.sampleTime) ? 10.f : 0.f);
  }
};

struct ClockDividerWidget : ModuleWidget {
  ClockDividerWidget(ClockDivider *module) {
    setModule(module);
    setPanel(APP->window->loadSvg(
        asset::plugin(pluginInstance, "res/ClockDivider.svg")));

    addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(
        createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(createWidget<ScrewSilver>(
        Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(createWidget<ScrewSilver>(Vec(
        box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));


    addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.00, 20.00)), module, ClockDivider::CLOCK_INPUT));

    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.00, 40.00)), module, ClockDivider::DIV2_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24, 60.800)), module, ClockDivider::DIV4_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24, 80.800)), module, ClockDivider::DIV8_OUTPUT));
    addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24, 100.800)), module, ClockDivider::DIV16_OUTPUT));
  }
};

Model *modelClockDivider =
    createModel<ClockDivider, ClockDividerWidget>("ClockDivider");
