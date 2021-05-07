const path = require('path');
const webpack = require('webpack');
const CleanWebpackPlugin = require('clean-webpack-plugin');
const CopyWebpackPlugin = require('copy-webpack-plugin');
const exec = require('child_process').exec;
const env = process.env.NODE_ENV;

module.exports = {
  mode: env || 'development',
  entry: {
    index: './src/index.js',
    apmode: './src/apmode.js'
  },
  plugins: [
    // Clean up our dist folder
    new CleanWebpackPlugin(['dist']),
    
    // Copy the static assets to our dist folder
    new CopyWebpackPlugin([
      { 
        from: '../pub', 
        to: './',
        context: './dist'
      }
    ]),
    
    // Make JQuery available to our code
    new webpack.ProvidePlugin({
      $: 'jquery',
      jQuery: 'jquery'
    }),
    
    // Clean up our arduino data folder
    new CleanWebpackPlugin(['data'], {
      root: __dirname + '../../arduino'
    }),
    
    // Copy the build to the hallway_monitor data directory
    {
      apply: (compiler) => {
        compiler.hooks.afterEmit.tap('AfterEmitPlugin', (compilation) => {
          exec(`bash post_build_deploy.sh`, (err, stdout, stderr) => {
            if (stdout) process.stdout.write(stdout);
            if (stderr) process.stderr.write(stderr);
          });
        });
      }
    }
  ],
  output: {
    path: path.resolve(__dirname, 'dist'),
    filename: '[name].js'
  },
  module: {
    rules: [
      { 
        test: /\.js$/, 
        exclude: /node_modules/, 
        loader: "babel-loader" 
      }
    ]
  },
  devServer: {
    contentBase: path.join(__dirname, 'dist'),
    compress: true,
    port: 9000,
  },
};