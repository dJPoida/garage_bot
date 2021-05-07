const path = require('path');
const webpack = require('webpack')
const { merge } = require('webpack-merge')

const paths = require('./webpack.paths')
const common = require('./webpack.config.common.js')

const tsConfigPath = path.resolve(__dirname, 'tsconfig.dev.json');

module.exports = merge(common, {
  // Set the mode to development or production
  mode: 'development',

  // Control how source maps are generated
  devtool: 'inline-source-map',

  // Spin up a server for quick development
  devServer: {
    historyApiFallback: true,
    contentBase: paths.dist,
    compress: true,
    hot: true,
    port: 3000
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
              sourceMap: true
            }
          },
        ],
      },
    ],
  },

  plugins: [
    // Only update what has changed on hot reload
    new webpack.HotModuleReplacementPlugin(),
  ],
})