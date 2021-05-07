const path = require('path');
const webpack = require('webpack');
const { CleanWebpackPlugin } = require('clean-webpack-plugin')
const CopyWebpackPlugin = require('copy-webpack-plugin')
const HtmlWebpackPlugin = require('html-webpack-plugin')
const PrettierPlugin = require('prettier-webpack-plugin')
const ESLintPlugin = require('eslint-webpack-plugin')

const packageJson = require('./package.json');
const entryPoints = require('./webpack.entrypoints');
const paths = require('./webpack.paths')

const appVersionSuffix = packageJson.version.replace(/\./g, '-');

const entry = (() => {
  const result = {};

  entryPoints.forEach((entryPoint) => {
    result[entryPoint.name] = [
      path.resolve(paths.src, `${entryPoint.jsFileName}.tsx`),
    ];
  });
  return result;
})();

module.exports = {
  // Specify multiple entry points for each app type
  entry,

  // Where webpack outputs the assets and bundles
  output: {
    path: paths.dist,
    publicPath: '/',
    filename: 'js/[name].[contenthash].bundle.js',
  },
  
  // Customize the webpack build process
  plugins: [
    // Provide some global variables to the client
    new webpack.DefinePlugin({
      // Put: 'client side variables here'
      __VERSION__: JSON.stringify(packageJson.version),
      __TITLE__: JSON.stringify(packageJson.displayName),
    }),

    // Removes/cleans build folders and unused assets when rebuilding
    new CleanWebpackPlugin(),

    // Copies files from target to destination folder
    // Copy other static assets to our dist folder
    new CopyWebpackPlugin({
      patterns: [{
        from: paths.public,
        to: paths.dist,
        toType: 'dir',
      }],
    }),

    // Use HTML Webpack Plugin to copy and populate our html templates
    // Generates deprecation warning: https://github.com/jantimon/html-webpack-plugin/issues/1501
    ...entryPoints.map((entryPoint) => new HtmlWebpackPlugin({
      template: path.resolve(paths.src, `${entryPoint.templateFilename}.html`),
      filename: path.resolve(paths.dist, `${entryPoint.outputFilename}.html`),
      chunks: [entryPoint.name],
      hash: true,
      templateParameters: {
        appVersionSuffix,
        appTitle: packageJson.displayName,
      },
    })),    

    // ESLint configuration
    new ESLintPlugin({
      files: ['.', 'src'],
      formatter: 'table',
    }),

    // Prettier configuration
    new PrettierPlugin(),
  ],

  resolve: {
    modules: [paths.src, 'node_modules'],
    extensions: ['.ts', '.tsx', '.js', '.jsx', '.json'],
    alias: {
      '@': paths.src,
    },
  },
}