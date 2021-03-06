const path = require('path');
const webpack = require('webpack');
const { merge } = require('webpack-merge');

const paths = require('./webpack.paths');
const common = require('./webpack.config.common.js');

const tsConfigPath = path.resolve(__dirname, 'tsconfig.dev.json');
const dotEnv = require('dotenv');

dotEnv.config();

module.exports = merge(common, {
  // Set the mode to development or production
  mode: 'development',

  // Where webpack outputs the assets and bundles
  output: {
    path: paths.dist,
    publicPath: '/',
    filename: 'js/[name].[contenthash].bundle.js',
  },

  // Control how source maps are generated
  devtool: 'inline-source-map',

  // Spin up a server for quick development
  devServer: {
    historyApiFallback: true,
    contentBase: paths.dist,
    compress: true,
    hot: true,
    host: process.env.APP_ADDRESS,
    port: 3000,
    stats: 'errors-only',
  },

  module: {
    rules: [
      {
        test: /\.ts(x?)$/,
        exclude: /node_modules/,
        loader: 'ts-loader',
        options: {
          configFile: tsConfigPath,
        },
      },
      // All output '.js' files will have any sourcemaps re-processed by 'source-map-loader'.
      {
        enforce: 'pre',
        test: /\.js$/,
        loader: 'source-map-loader',
      },
      // Styles: Inject CSS into the head with source maps
      {
        test: /\.(scss|css)$/,
        use: [
          'style-loader',
          {
            loader: 'css-loader',
            options: {
              sourceMap: true,
              // importLoaders: 1,
              // modules: true,
              url: false,
            },
          },
          {
            loader: 'sass-loader',
            options: {
              sourceMap: true,
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
      __ENVIRONMENT__: JSON.stringify('development'), 
      __DEVICE_ADDRESS__: JSON.stringify(process.env.DEVICE_ADDRESS),
    }),

    // Only update what has changed on hot reload
    new webpack.HotModuleReplacementPlugin(),
  ],
});
