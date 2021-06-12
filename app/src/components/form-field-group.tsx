import React from 'react';

export type FormFieldGroupProps = {
  legend?: string;
};

export const FormFieldGroup: React.FC<FormFieldGroupProps> = (props) => {
  const { children, legend } = props;

  return (
    <div className="form-field-group">
      {legend && <legend>{legend}</legend>}
      {children}
    </div>
  );
};
