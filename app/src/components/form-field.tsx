import React from 'react';
import classNames from 'classnames';
import { ToolTip } from './tooltip';

export type TextboxChangeFunction = (
  fieldName: string,
  newValue: null | string
) => void;
export type NumberChangeFunction = (
  fieldName: string,
  newValue: null | number,
) => void;
export type CheckboxChangeFunction = (
  fieldName: string,
  newValue: boolean
) => void;

export type FormFieldProps = {
  className?: string;
  labelClassName?: string;
  id: string;
  fieldName: string;
  label: string;
  toolTip?: string;
} & (
  | {
      type: 'text';
      value: null | string;
      nullable?: undefined,
      minValue?: undefined,
      maxValue?: undefined,
      onChange: TextboxChangeFunction;
    }
  | {
      type: 'number';
      value: null | number;
      nullable?: boolean,
      minValue?: number,
      maxValue?: number,
      onChange: NumberChangeFunction;
    }
  | {
      type: 'checkbox';
      value: boolean;
      nullable?: undefined,
      minValue?: undefined,
      maxValue?: undefined,
      onChange: CheckboxChangeFunction;
    }
);

export const FormField: React.FC<FormFieldProps> = (props) => {
  const {
    className,
    labelClassName,
    id,
    fieldName,
    label,
    type,
    value,
    toolTip,
    nullable = false,
    minValue,
    maxValue,
    onChange,
  } = props;

  return (
    <>
      {/* eslint-disable-next-line jsx-a11y/anchor-is-valid, jsx-a11y/interactive-supports-focus, jsx-a11y/no-static-element-interactions, jsx-a11y/click-events-have-key-events, jsx-a11y/no-noninteractive-element-interactions */}
      <label
        className={classNames('form-field', labelClassName)}
        htmlFor={id}
        onClick={(e) => { e.stopPropagation(); e.preventDefault(); }}
      >
        <span>{label}</span>
        {toolTip && <ToolTip id={`tooltip_${id}`} tip={toolTip} />}
      </label>

      {/* Checkbox */}
      {type === 'checkbox' && (
        <label
          className={classNames('checkbox form-field', className)}
        >
          <input
            key={id}
            id={id}
            name={fieldName}
            type={type}
            checked={(value as null | boolean) ?? false}
            onChange={(e) => (onChange as CheckboxChangeFunction)(fieldName, e.target.checked)}
          />
          <span>
            {(value as null | boolean) ?? false ? 'Yep' : 'Nup'}
          </span>
        </label>
      )}

      {/* Numeric Input */}
      {type === 'number' && (
        <input
          key={id}
          className={classNames('form-field', className)}
          id={id}
          name={fieldName}
          type="number"
          value={(value ?? '').toString()}
          onChange={(e) => {
            const newInputValue: null | string = e.target.value ?? (nullable ? null : '0');
            let newValue: null | number = (newInputValue ? Number(newInputValue) : null);

            if (newValue && (maxValue !== undefined)) {
              newValue = Math.min(newValue, maxValue);
            }
            if (newValue && (minValue !== undefined)) {
              newValue = Math.max(newValue, minValue);
            }
            (onChange as NumberChangeFunction)(
              fieldName,
              newValue,
            );
          }}
        />
      )}

      {/* Textbox */}
      {type === 'text' && (
        <input
          key={id}
          className={classNames('form-field', className)}
          id={id}
          name={fieldName}
          type="text"
          value={(value ?? '').toString()}
          onChange={(e) => (onChange as TextboxChangeFunction)(
            fieldName,
            (e.target.value ?? '') !== '' ? e.target.value ?? '' : null,
          )}
        />
      )}
    </>
  );
};
