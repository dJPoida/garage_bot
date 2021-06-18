import React from 'react';
import { AN_ICON } from '../constants/icon.const';

export type PageTitleProps = {
  title: string,
  icon?: AN_ICON,
}

export const PageTitle: React.FC<PageTitleProps> = (props) => {
  const {
    title,
    icon,
  } = props;
  return (
    <div className="page-title">
      {icon && (
        <span className={icon} />
      )}
      <span>{title}</span>
    </div>
  );
};
