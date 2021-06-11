import React from "react";
import ReactTooltip from "react-tooltip";
import { ICON } from "../constants/icon.const";

export type ToolTipProps = {
  tip: string;
};

export const ToolTip: React.FC<ToolTipProps> = (props) => {
  const { tip } = props;

  return (
    <>
      <a className="tool-tip" data-tip={tip}>
        <span className={ICON.INFO} />
      </a>
      <ReactTooltip place="top" type="light" effect="solid" multiline />
    </>
  );
};
