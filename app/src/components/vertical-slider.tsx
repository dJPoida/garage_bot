import classNames from 'classnames';
import React, { useState } from 'react';
import ReactSlider from 'react-slider';

export type VerticalSliderProps = {
  label?: string,
  minValue?: number,
  maxValue?: number,
  value: number,
  backgroundValue?: number,
  passed?: boolean,
  onAfterChange: (newValue: number) => void
}

export const VerticalSlider: React.FC<VerticalSliderProps> = (props) => {
  const {
    label,
    minValue = 0,
    maxValue = 100,
    backgroundValue = 0,
    value,
    passed,
    onAfterChange,
  } = props;

  return (
    <div className="vertical-slider">
      {label && (
        <label>
          {label}
        </label>
      )}
      <div className="slider-wrapper">
        <div className="background-wrapper">
          <div className="background" style={{ height: `${(backgroundValue / maxValue) * 100}%` }} />
          <div className="background clip" style={{ height: `${backgroundValue > value ? ((value / maxValue) * 100) : ((backgroundValue / maxValue) * 100)}%` }} />
        </div>
        <ReactSlider
          className="react-slider-vertical"
          thumbClassName="slider-thumb"
          trackClassName="slider-track"
          renderThumb={(thumbProps, state) => <div {...thumbProps}>{state.valueNow}</div>}
          orientation="vertical"
          pearling
          invert
          min={minValue}
          max={maxValue}
          value={Math.max(Math.min(value, maxValue), minValue)}
          onAfterChange={(newValue) => onAfterChange(newValue)}
        />
      </div>
      {(passed !== null) && (
        <div className={classNames('passed', {
          success: passed,
          fail: !passed,
        })}
        >
          {(passed === true) && '✔'}
          {(passed === false) && '✖'}
        </div>
      )}
    </div>
  );
};
