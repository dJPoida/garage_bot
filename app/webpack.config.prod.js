const path = require('path');
const webpack = require('webpack');
const MiniCssExtractPlugin = require('mini-css-extract-plugin');
const CssMinimizerPlugin = require('css-minimizer-webpack-plugin');
const FileManagerPlugin = require('filemanager-webpack-plugin');
const { merge } = require('webpack-merge');

const packageJson = require('./package.json');
const paths = require('./webpack.paths');
const common = require('./webpack.config.common.js');

const appVersionSuffix = packageJson.version.replace(/\./g, '-');
const tsConfigPath = path.resolve(__dirname, 'tsconfig.prod.json');
const arduinoDataPath = path.resolve(__dirname, '../arduino/garage_bot/data/');

module.exports = merge(common, {
  mode: 'production',
  
  // Where webpack outputs the assets and bundles
  output: {
    path: paths.dist,
    publicPath: '/',
    filename: `js/[name].${appVersionSuffix}.bnd.js`,
  },
  
  devtool: false,
  
  module: {
    rules: [
      {
        test: /\.ts(x?)$/,
        exclude: /node_modules/,
        loader: 'ts-loader',
        options: {
          configFile: tsConfigPath
        },
      },
      {
        test: /\.(scss|css)$/,
        use: [
          MiniCssExtractPlugin.loader,
          {
            loader: 'css-loader',
            options: {
              // importLoaders: 2,
              sourceMap: false,
              // modules: true,
              url: false,
            },
          },
          {
            loader: 'sass-loader',
            options: {
              sourceMap: false,
            },
          },
        ],
      },
    ],
  },
  plugins: [
    // Provide some global variables to the client
    new webpack.DefinePlugin({
      // Put: 'client side variables here'
      __ENVIRONMENT__: JSON.stringify("production"),
    }),

    // Extracts CSS into separate files
    new MiniCssExtractPlugin({
      filename: 'styles/[name].[contenthash].css',
      chunkFilename: '[id].css',
    }),

    // Copy the build to the arduino data directory
    new FileManagerPlugin({
      events: {
        onEnd: {
          // Delete the existing directory
          delete: [{
            source: arduinoDataPath,
            options: {
              force: true,
            },
          }],
          // Re-create the directory
          mkdir: [arduinoDataPath],
          copy: [
            // Copy the dist output into the directory
            { source: paths.dist, destination: arduinoDataPath },
            // Copy the local config.json to the arduino esp data path
            { source: './config.json', destination: `${arduinoDataPath}/config.json` }
          ],
        },
      },
    }),
  ],

  optimization: {
    minimize: true,
    minimizer: [new CssMinimizerPlugin(), '...'],
    runtimeChunk: {
      name: 'runtime',
    },
  },

  performance: {
    hints: false,
    maxEntrypointSize: 512000,
    maxAssetSize: 512000,
  },
});
