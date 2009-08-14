require 'test/unit'
require 'rbclips'

class Test_Base < Test::Unit::TestCase
  def test_exists
    assert Clips.constants.member?(:Environment)
  end

  def test_internal_buffer
    num = Clips::Environment.all.size
    Clips::Environment.new
    Clips::Environment.new
    Clips::Environment.new
    assert_equal num + 3, Clips::Environment.all.size
  end

  def test_current
    # By default there should be one default environment
    assert_not_nil Clips::Environment.current

    # Setting default environment
    a = Clips::Environment.new
    a.set_current
    assert_equal a.to_s, Clips::Environment.current.to_s
  end
end
