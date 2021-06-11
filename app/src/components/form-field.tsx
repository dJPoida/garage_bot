import React from "react";
import classNames from "classnames";
import { ToolTip } from "./tooltip";

export type TextboxChangeFunction = (
  fieldName: string,
  newValue: null | string
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
      type: "text";
      value: null | string;
      onChange: TextboxChangeFunction;
    }
  | {
      type: "checkbox";
      value: boolean;
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
    onChange,
  } = props;

  return (
    <>
      <label className={classNames("form-field", labelClassName)} htmlFor={id}>
        <span>{label}</span>
        {toolTip && <ToolTip tip={toolTip} />}
      </label>

      {/* Checkbox */}
      {type === "checkbox" && (
        <input
          key={id}
          className={classNames("form-field", className)}
          id={id}
          name={fieldName}
          type={type}
          checked={(value as null | boolean) ?? false}
          onChange={(e) =>
            (onChange as CheckboxChangeFunction)(fieldName, e.target.checked)
          }
        />
      )}

      {/* Textbox */}
      {type === "text" && (
        <input
          key={id}
          className={classNames("form-field", className)}
          id={id}
          name={fieldName}
          type={type}
          value={(value ?? "").toString()}
          onChange={(e) =>
            (onChange as TextboxChangeFunction)(
              fieldName,
              (e.target.value ?? "") !== "" ? e.target.value ?? "" : null
            )
          }
        />
      )}
    </>
  );
};
