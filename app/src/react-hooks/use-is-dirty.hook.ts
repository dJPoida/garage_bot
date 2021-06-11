import { useState } from "react";

/**
 * Simple hook to flag that a form has been touched and is dirty
 *
 * usage:
 *    const [isDirty, setDirty] = useIsDirty();
 *    if (isDirty) {
 *      // do Something different when the form is dirty
 *    }
 *    <myForm
 *      onChange={setDirty}
 *    />
 */
export const useIsDirty = (): [boolean, (isDirty?: boolean) => void] => {
  const [isDirty, setIsDirty] = useState(false);

  const setDirty = (isDirty?: boolean) => setIsDirty(isDirty !== false);

  return [isDirty, setDirty];
};
