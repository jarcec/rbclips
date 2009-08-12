require 'test/unit'
require 'rbclips'


class Test_Module < Test::Unit::TestCase
  def test_is_a
    assert Clips.is_a?(Module)
  end
end
