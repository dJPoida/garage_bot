module.exports = {
  extends: [
    'airbnb',
    'plugin:@typescript-eslint/recommended',
    'plugin:react/recommended',
    'plugin:css-modules/recommended',
  ],
  parser: '@typescript-eslint/parser',
  plugins: [
    'react',
    'jsx-a11y',
    'import',
    'css-modules',
    'react-hooks',
    '@typescript-eslint',
    'prettier',
  ],
  env: {
    browser: true,
    es6: true,
  },
  settings: {
    'import/parsers': {
      '@typescript-eslint/parser': ['.ts', '.tsx'],
    },
    'import/resolver': {
      typescript: {},
    },
  },
  parserOptions: {
    ecmaVersion: 2020,
    sourceType: 'module',
    ecmaFeatures: {
      jsx: true,
      classes: true,
      defaultParams: true,
    },
  },
  rules: {
    '@typescript-eslint/ban-ts-ignore': 'off',
    '@typescript-eslint/camelcase': 'off',
    '@typescript-eslint/class-name-casing': 'off',
    '@typescript-eslint/explicit-function-return-type': 'off',
    '@typescript-eslint/interface-name-prefix': 'off',
    '@typescript-eslint/no-empty-interface': 'off',
    '@typescript-eslint/no-this-alias': 'off',
    '@typescript-eslint/no-use-before-define': ['error', { variables: false }],
    'brace-style': 'off',
    'class-methods-use-this': 'off',
    'comma-dangle': ['error', 'always-multiline'],
    'comma-spacing': ['error', { before: false, after: true }],
    'import/extensions': ['error', 'ignorePackages', { js: 'never', jsx: 'never', ts: 'never', tsx: 'never' }],
    'import/no-extraneous-dependencies': [2, { devDependencies: ['**/test.tsx', '**/test.ts'] }],
    'import/prefer-default-export': 'off',
    'jsx-a11y/label-has-associated-control': 'off',
    'linebreak-style': 'off',
    'lines-between-class-members': 'off',
    'max-classes-per-file': 'off',
    'max-len': ['error', { code: 180, ignoreComments: true }],
    'no-await-in-loop': 'off',
    'no-console': ['error', { allow: ['warn', 'error', 'info'] }],
    'no-multiple-empty-lines': ['error', { max: 2 }],
    'no-nested-ternary': 'off',
    'no-restricted-syntax': 'off',
    'no-underscore-dangle': 'off',
    'no-use-before-define': 'off',
    'no-useless-constructor': 'off',
    'object-curly-newline': 'off',
    'prefer-arrow-callback': 'off',
    'react-hooks/exhaustive-deps': 'error',
    'react-hooks/rules-of-hooks': 'error',
    'react/destructuring-assignment': ['warn', 'always', { ignoreClassFields: true }],
    'react/jsx-filename-extension': [2, { extensions: ['.js', '.jsx', '.ts', '.tsx'] }],
    'react/jsx-props-no-spreading': 'off',
    'react/prop-types': 'off',
    camelcase: 'off',
  },
  overrides: [
    {
      files: ['*.js', '*.jsx'],
      rules: {
        '@typescript-eslint/explicit-module-boundary-types': ['off'],
      },
    },
  ],
};
