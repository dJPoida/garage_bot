import React from 'react';
import ReactTooltip from 'react-tooltip';
import { ICON } from '../constants/icon.const';

export type ToolTipProps = {
  id: string,
  tip: string;
};

export const ToolTip: React.FC<ToolTipProps> = (props) => {
  const { tip, id } = props;

  return (
    <>
      {/* eslint-disable-next-line jsx-a11y/anchor-is-valid */}
      <a
        className="tool-tip"
        data-tip
        data-for={id}
      >
        <span className={ICON.INFO} />
      </a>
      <ReactTooltip id={id} place="top" type="light" effect="solid" multiline>
        {tip}
      </ReactTooltip>
    </>
  );
};
