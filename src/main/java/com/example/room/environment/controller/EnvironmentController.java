package com.example.room.environment.controller;

import cn.hutool.core.bean.BeanUtil;
import com.alibaba.excel.EasyExcel;
import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.example.room.environment.entity.Environment;
import com.example.room.environment.entity.dto.EnvironmentExportVO;
import com.example.room.environment.entity.dto.EnvironmentQuery;
import com.example.room.environment.entity.dto.EnvironmentStatisticsQuery;
import com.example.room.environment.entity.dto.EnvironmentVO;
import com.example.room.environment.service.EnvironmentService;
import com.example.room.util.Result;
import io.swagger.annotations.Api;
import io.swagger.annotations.ApiOperation;
import io.swagger.annotations.ApiParam;
import org.springframework.security.access.prepost.PreAuthorize;
import org.springframework.web.bind.annotation.*;

import javax.annotation.Resource;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.net.URLEncoder;
import java.util.Date;
import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;

@Api(description = "环境监测数据管理")
@RestController
@RequestMapping("/service/environment")
@CrossOrigin
public class EnvironmentController {

    @Resource
    private EnvironmentService environmentService;

    @ApiOperation(value = "所有环境监测数据列表")
    @PreAuthorize("hasAuthority('environment.list')")
    @GetMapping("findAll")
    public Result<List<Environment>> findAll() {
        return Result.ok(environmentService.list());
    }

    @ApiOperation(value = "逻辑删除")
    @PreAuthorize("hasAuthority('environment.remove')")
    @DeleteMapping("{id}")
    public Result<String> removeEnvironment(@PathVariable Long id) {
        return environmentService.removeById(id) ? Result.ok() : Result.fail();
    }

    @ApiOperation(value = "条件查询分页方法")
    @PreAuthorize("hasAuthority('environment.list')")
    @PostMapping("pageEnvironmentCondition")
    public Result<Page<EnvironmentVO>> pageEnvironmentCondition(@RequestBody(required = false) EnvironmentQuery environmentQuery) {
        Page<Environment> environmentPage = environmentService.pageQuery(environmentQuery);
        List<EnvironmentVO> voList = BeanUtil.copyToList(environmentPage.getRecords(), EnvironmentVO.class);
        Page<EnvironmentVO> voPage = new Page<>(environmentPage.getCurrent(), environmentPage.getSize(), environmentPage.getTotal());
        voPage.setRecords(voList);
        return Result.ok(voPage);
    }

    @ApiOperation(value = "统计分析")
    @PreAuthorize("hasAuthority('environment.list')")
    @PostMapping("statistics")
    public Result<List<Environment>> getStatistics(@RequestBody(required = false) EnvironmentStatisticsQuery query) {
        return Result.ok(environmentService.getStatistics(query));
    }

    @ApiOperation(value = "获取最新一条数据")
    @PreAuthorize("hasAuthority('realTime.index')")
    @GetMapping("getLastEnvironment")
    public Result<Environment> getLastEnvironment(@RequestParam("deviceKey") String deviceKey) {
        return Result.ok(environmentService.getLastData(deviceKey));
    }

    @ApiOperation("导出环境监测数据")
    @PostMapping("export")
    public void export(@RequestBody(required = false) EnvironmentQuery environmentQuery,
                       HttpServletResponse response) {
        try {
            environmentQuery.setCurrentPage(1);
            environmentQuery.setPageSize(Integer.MAX_VALUE);
            Page<Environment> pageResult = environmentService.pageQuery(environmentQuery);
            List<EnvironmentExportVO> exportVOList = BeanUtil.copyToList(pageResult.getRecords(), EnvironmentExportVO.class);
            AtomicInteger index = new AtomicInteger(1);
            exportVOList.forEach(vo -> vo.setIndex(index.getAndIncrement()));

            response.setContentType("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet");
            response.setCharacterEncoding("utf-8");
            String fileName = URLEncoder.encode("环境监测数据", "UTF-8").replaceAll("\\+", "%20");
            response.setHeader("Content-disposition", "attachment;filename*=utf-8''" + fileName + ".xlsx");
            EasyExcel.write(response.getOutputStream(), EnvironmentExportVO.class)
                    .sheet("环境数据")
                    .doWrite(exportVOList);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    @ApiOperation("添加环境监测数据")
    @PreAuthorize("hasAuthority('environment.add')")
    @PostMapping("addEnvironment")
    public Result<String> addEnvironment(@RequestBody Environment environment) {
        environment.setGmtCreate(new Date());
        return environmentService.save(environment) ? Result.ok() : Result.fail();
    }

    @ApiOperation("根据ID查询环境监测数据")
    @PreAuthorize("hasAuthority('environment.list')")
    @GetMapping("getEnvironment/{id}")
    public Result<Environment> getEnvironment(@PathVariable Long id) {
        return Result.ok(environmentService.getById(id));
    }

    @ApiOperation("修改环境监测数据")
    @PreAuthorize("hasAuthority('environment.update')")
    @PostMapping("updateEnvironment")
    public Result<String> updateEnvironment(@RequestBody Environment environment) {
        return environmentService.updateById(environment) ? Result.ok() : Result.fail("修改失败");
    }
}
