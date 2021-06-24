import React from 'react';

export type ModalProps = {
  id: string,
  visible: boolean,
  title?: string,
  confirmClass?: 'primary' | 'info' | 'success' | 'warning' | 'danger',
  confirmLabel: string,
  onClose: () => void,
  onConfirm: () => void,
}

export const Modal:React.FC<ModalProps> = (props) => {
  const {
    id,
    visible = false,
    children,
    title = 'Are you sure?',
    confirmClass = 'primary',
    confirmLabel = 'Yes, Reboot',
    onClose,
    onConfirm,
  } = props;

  return (
    <div className="modal-wrapper">
      <input
        type="checkbox"
        id={id}
        className="modal"
        checked={visible}
        onChange={(e) => {
          if (!e.target.checked) {
            onClose();
          }
        }}
      />
      <div>
        <div className="card">
          <label htmlFor={id} className="modal-close" />
          <h3 className="section">
            {title}
          </h3>
          <div className="section">
            {children}
          </div>
          <div className="button-row">
            <button
              type="button"
              onClick={onConfirm}
              className={confirmClass}
            >
              {confirmLabel}
            </button>
          </div>
        </div>
      </div>
    </div>
  );
};
