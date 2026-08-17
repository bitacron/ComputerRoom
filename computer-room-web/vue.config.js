'use strict'
const path = require('path')
const { defineConfig } = require('@vue/cli-service')

function resolve(dir) {
  return path.join(__dirname, dir)
}

const BASE_API = process.env.BASE_API || 'http://localhost:8080'
const port = process.env.port || process.env.npm_config_port || 9528

module.exports = defineConfig({
  transpileDependencies: true,
  lintOnSave: false,
  productionSourceMap: false,
  publicPath: '/',
  outputDir: 'dist',
  assetsDir: 'static',
  devServer: {
    host: 'localhost',
    port,
    open: true,
    client: {
      overlay: {
        warnings: false,
        errors: true
      }
    }
  },
  css: {
    loaderOptions: {
      sass: {
        sassOptions: {
          quietDeps: true
        }
      }
    }
  },
  configureWebpack: {
    name: '数据中心机房监控',
    resolve: {
      alias: {
        '@': resolve('src')
      },
      fallback: {
        // SidebarItem 使用 path.resolve，Webpack5 需浏览器 polyfill
        path: require.resolve('path-browserify'),
        net: false,
        tls: false,
        fs: false,
        crypto: false,
        stream: false
      }
    }
  },
  chainWebpack(config) {
    // 注入与旧工程一致的 BASE_API，业务代码无需改 process.env.BASE_API
    config.plugin('define').tap(args => {
      args[0]['process.env'].BASE_API = JSON.stringify(BASE_API)
      return args
    })

    // 页面标题
    config.plugin('html').tap(args => {
      args[0].title = '数据中心机房监控'
      return args
    })

    // svg icon：src/icons 走 sprite，其余 svg 走默认 file-loader
    config.module.rule('svg').exclude.add(resolve('src/icons')).end()
    config.module
      .rule('icons')
      .test(/\.svg$/)
      .include.add(resolve('src/icons'))
      .end()
      .use('svg-sprite-loader')
      .loader('svg-sprite-loader')
      .options({
        symbolId: 'icon-[name]'
      })
      .end()
  }
})
