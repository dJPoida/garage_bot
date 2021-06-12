import { useEffect, useRef } from 'react';

/**
 * @description
 * Hook to keep track of a previous value that is watched using useEffect
 */
export function usePreviousValue<T>(value: T): T | undefined {
  const ref = useRef<T>();
  useEffect(() => {
    ref.current = value;
  });
  return ref.current;
}
