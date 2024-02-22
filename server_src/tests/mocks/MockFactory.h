#pragma once

#include <gmock/gmock.h>

#include <functional>
#include <memory>

template <typename Product, typename... ArgTypes>
class MockFactory;

template <typename Product, typename... ArgTypes>
struct MockFactory<Product(ArgTypes...)> : public testing::MockFunction<std::unique_ptr<Product>(ArgTypes...)>
{
    MockFactory()
    {
        originalFactory = Product::create;
        Product::create = this->AsStdFunction();
    }

    ~MockFactory()
    {
        Product::create = originalFactory;
    }

    std::function<std::unique_ptr<Product>(ArgTypes...)> originalFactory;
};
